#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
using namespace yulast;

// helpers

bool isUnconditionalTerminator(llvm::Instruction *i) {
  if (!i->isTerminator())
    return false;
  else {
    auto brInst = llvm::dyn_cast<llvm::BranchInst>(i);
    if (brInst && brInst->isConditional())
      return false;
    auto swInst = llvm::dyn_cast<llvm::SwitchInst>(i);
    if (swInst)
      return false;
    return true;
  }
}

LLVMCodegenVisitor::LLVMCodegenVisitor() {
  // LLVM functions and datastructures
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("yul", *TheContext);
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
  funCallHelper = std::make_unique<YulFunctionCallHelper>(*this);
  funDefHelper = std::make_unique<YulFunctionDefinitionHelper>(*this);
  FPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
  FPM->add(llvm::createLoopSimplifyPass());
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
}

void LLVMCodegenVisitor::runFunctionDeclaratorVisitor(YulContractNode &node) {
  FunctionDeclaratorVisitor declVisitor(*TheContext, *TheModule);
  declVisitor.visit(node);
}

llvm::AllocaInst *LLVMCodegenVisitor::CreateEntryBlockAlloca(
    llvm::Function *TheFunction, const std::string &VarName, llvm::Type *type) {

  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin());
  if (!type)
    type = llvm::Type::getIntNTy(*TheContext, 256);
  return TmpB.CreateAlloca(type, 0, VarName.c_str());
}

llvm::GlobalVariable *
LLVMCodegenVisitor::CreateGlobalStringLiteral(std::string literalValue,
                                              std::string literalName) {
  stringLiteralNames[literalValue] = literalName;
  return Builder->CreateGlobalString(literalValue, literalName);
}

llvm::Module &LLVMCodegenVisitor::getModule() { return *TheModule; }

llvm::IRBuilder<> &LLVMCodegenVisitor::getBuilder() { return *Builder; }

llvm::LLVMContext &LLVMCodegenVisitor::getContext() { return *TheContext; }

llvm::StringMap<llvm::AllocaInst *> &LLVMCodegenVisitor::getNamedValuesMap() {
  return NamedValues;
}

// visitors

void LLVMCodegenVisitor::visitYulAssignmentNode(YulAssignmentNode &node) {
  for (auto &var : node.getLHSIdentifiers()) {
    std::string lvalname = var->getIdentfierValue();
    llvm::AllocaInst *lval = NamedValues[lvalname];
    if (lval == nullptr) {
      llvm::WithColor::error() << "Assignment Node: lval not found " << lvalname
                               << " in " << node.to_string();
      exit(EXIT_FAILURE);
    }
    llvm::Value *rval = visit(node.getRHSExpression());
    Builder->CreateStore(rval, lval, false);
  }
}
void LLVMCodegenVisitor::visitYulBlockNode(YulBlockNode &node) {
  for (auto &s : node.getStatements()) {
    visit(*s);
  }
}

void LLVMCodegenVisitor::visitYulBreakNode(YulBreakNode &node) {
  assert(loopControlFlowBlocks.size() > 0 && "Break not in loop");
  llvm::BasicBlock *contBB = std::get<0>(loopControlFlowBlocks.top());
  Builder->CreateBr(contBB);
}

void LLVMCodegenVisitor::visitYulCaseNode(YulCaseNode &node) {
  visit(node.getThenBody());
}
void LLVMCodegenVisitor::visitYulContinueNode(YulContinueNode &node) {
  assert(loopControlFlowBlocks.size() > 0 && "Break not in loop");
  llvm::BasicBlock *contBB = std::get<1>(loopControlFlowBlocks.top());
  Builder->CreateBr(contBB);
}
void LLVMCodegenVisitor::visitYulContractNode(YulContractNode &node) {
  runFunctionDeclaratorVisitor(node);
  constructStruct(node);
  currentContract = &node;
  for (auto &f : node.getFunctions()) {
    visit(*f);
  }
}

void LLVMCodegenVisitor::visitYulDefaultNode(YulDefaultNode &node) {
  visit(node.getThenBody());
}

void LLVMCodegenVisitor::visitYulForNode(YulForNode &node) {
  llvm::Function *enclosingFunction = Builder->GetInsertBlock()->getParent();
  // initializetion code gen can happen in current basic block
  // create termination condtition basic block
  llvm::BasicBlock *condBB =
      llvm::BasicBlock::Create(*TheContext, "for-cond", enclosingFunction);
  llvm::BasicBlock *bodyBB =
      llvm::BasicBlock::Create(*TheContext, "for-body", enclosingFunction);
  llvm::BasicBlock *contBB =
      llvm::BasicBlock::Create(*TheContext, "for-cont", enclosingFunction);
  llvm::BasicBlock *incrBB = llvm::BasicBlock::Create(*TheContext, "for-incr");

  visit(node.getInitializationNode());
  Builder->CreateBr(condBB);

  loopControlFlowBlocks.push(std::make_tuple(contBB, incrBB));
  Builder->SetInsertPoint(condBB);
  llvm::Value *cond = Builder->CreateCast(llvm::Instruction::CastOps::Trunc,
                                          visit(node.getCondition()),
                                          llvm::Type::getInt1Ty(*TheContext));
  Builder->CreateCondBr(cond, bodyBB, contBB);

  Builder->SetInsertPoint(bodyBB);
  visit(node.getBody());
  Builder->CreateBr(incrBB);
  loopControlFlowBlocks.pop();

  // create increment basic block,
  // creating a separate basic block because cont can jump here
  enclosingFunction->getBasicBlockList().push_back(incrBB);
  Builder->SetInsertPoint(incrBB);
  visit(node.getIncrementNode());
  Builder->CreateBr(condBB);

  // Reorder the cont basic block
  enclosingFunction->getBasicBlockList().remove(contBB);
  enclosingFunction->getBasicBlockList().push_back(contBB);

  Builder->SetInsertPoint(contBB);
}

llvm::Value *
LLVMCodegenVisitor::visitYulFunctionCallNode(YulFunctionCallNode &node) {
  return funCallHelper->visitYulFunctionCallNode(node);
}

void LLVMCodegenVisitor::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode &node) {
  funDefHelper->visitYulFunctionDefinitionNode(node);
}

llvm::Value *
LLVMCodegenVisitor::visitYulIdentifierNode(YulIdentifierNode &node) {
  llvm::Type *loadType = llvm::Type::getIntNTy(*TheContext, 256);
  llvm::AllocaInst *ptr;
  if (NamedValues.find(node.getIdentfierValue()) == NamedValues.end()) {
    for (auto &arg : currentFunction->args()) {
      if (!std::string(arg.getName()).compare(node.getIdentfierValue())) {
        return &arg;
      }
    }
  } else {
    ptr = NamedValues[node.getIdentfierValue()];
    loadType = ptr->getAllocatedType();
    return Builder->CreateLoad(loadType, ptr, node.getIdentfierValue());
  }
  assert(false && "Referenced undefined identifier");
  return nullptr;
}

void LLVMCodegenVisitor::visitYulIfNode(YulIfNode &node) {
  llvm::BasicBlock *thenBlock =
      llvm::BasicBlock::Create(*TheContext, "if-taken-body", currentFunction);
  llvm::BasicBlock *contBlock =
      llvm::BasicBlock::Create(*TheContext, "if-not-taken-body");
  llvm::Value *cond = visit(node.getCondition());
  cond = Builder->CreateCast(llvm::Instruction::CastOps::Trunc, cond,
                             llvm::IntegerType::getInt1Ty(*TheContext));

  // create actual branch on condition
  Builder->CreateCondBr(cond, thenBlock, contBlock);

  // emit then
  Builder->SetInsertPoint(thenBlock);
  visit(node.getThenBody());
  llvm::Instruction *i = &(Builder->GetInsertBlock()->getInstList().back());

  /**
   * @brief if last instruction is an uncoditional terminator, add a jump from
   * then-body to join node(not-taken-body)
   *
   */
  if (i && !isUnconditionalTerminator(i)) {
    Builder->CreateBr(contBlock);
  }

  // merge node
  currentFunction->getBasicBlockList().push_back(contBlock);
  Builder->SetInsertPoint(contBlock);
}
void LLVMCodegenVisitor::visitYulLeaveNode(YulLeaveNode &node) {}
llvm::Value *
LLVMCodegenVisitor::visitYulNumberLiteralNode(YulNumberLiteralNode &node) {
  return llvm::ConstantInt::get(*TheContext, node.getLiteralValue());
}
llvm::Value *
LLVMCodegenVisitor::visitYulStringLiteralNode(YulStringLiteralNode &node) {
  llvm::SHA1 hasher;
  hasher.update(node.getLiteralValue());
  std::string literalName =
      "str_lit_" + llvm::encodeBase64(hasher.final()).substr(0, 6);
  if (auto x = stringLiteralNames.find(node.getLiteralValue()) !=
               stringLiteralNames.end()) {
    std::string globalName = stringLiteralNames[node.getLiteralValue()];
    return TheModule->getOrInsertGlobal(globalName,
                                        llvm::Type::getInt8Ty(*TheContext));
  }
  return CreateGlobalStringLiteral(node.getLiteralValue(), literalName);
}
void LLVMCodegenVisitor::visitYulSwitchNode(YulSwitchNode &node) {
  llvm::Value *cond = visit(node.getCondition());
  llvm::BasicBlock *defaultBlock =
      llvm::BasicBlock::Create(*TheContext, "default");
  llvm::BasicBlock *cont =
      llvm::BasicBlock::Create(*TheContext, "-switch-cont");

  llvm::SwitchInst *sw =
      Builder->CreateSwitch(cond, defaultBlock, node.getCases().size() + 1);

  for (auto &c : node.getCases()) {
    llvm::BasicBlock *caseBB = llvm::BasicBlock::Create(
        *TheContext, "case-" + c->getCondition()->to_string(), currentFunction);
    llvm::APInt &literal = c->getCondition()->getLiteralValue();
    sw->addCase(llvm::ConstantInt::get(*TheContext, literal), caseBB);
    Builder->SetInsertPoint(caseBB);
    visit(*c);
    /**
     * @brief if last instruction is an uncoditional terminator, add a jump from
     * then-body to join node(not-taken-body)
     *
     */
    llvm::Instruction *i = &(Builder->GetInsertBlock()->getInstList().back());
    if (i && !isUnconditionalTerminator(i)) {
      Builder->CreateBr(cont);
    }
  }

  currentFunction->getBasicBlockList().push_back(defaultBlock);
  Builder->SetInsertPoint(defaultBlock);
  if (node.hasDefaultNode()) {
    visit(node.getDefaultNode());
  }
  Builder->CreateBr(cont);
  currentFunction->getBasicBlockList().push_back(cont);
  Builder->SetInsertPoint(cont);
}
void LLVMCodegenVisitor::visitYulVariableDeclarationNode(
    YulVariableDeclarationNode &node) {
  for (auto &id : node.getVars()) {
    if (node.hasValue()) {
      llvm::Value *constant = visit(node.getValue());
      codeGenForOneVarDeclaration(*id, constant->getType());
      llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
      Builder->CreateStore(constant, lval);
    } else {
      codeGenForOneVarDeclaration(*id, nullptr);
    }
  }
}
void LLVMCodegenVisitor::codeGenForOneVarDeclaration(YulIdentifierNode &id,
                                                     llvm::Type *type) {
  if (NamedValues[id.getIdentfierValue()] == nullptr) {

    llvm::AllocaInst *v =
        CreateEntryBlockAlloca(currentFunction, id.getIdentfierValue(), type);
    NamedValues[id.getIdentfierValue()] = v;
  }
}

void LLVMCodegenVisitor::constructStruct(YulContractNode &node) {
  if (node.getStructFieldOrder().size() == 0)
    return;
  std::vector<llvm::Type *> memberTypes;
  for (auto &field : node.getStructFieldOrder()) {
    std::string typeStr = std::get<0>(node.getTypeMap()[field]);
    int bitWidth = std::get<1>(node.getTypeMap()[field]);
    llvm::Type *type;
    if (bitWidth != 0)
      type = getTypeByBitwidth(bitWidth);
    else {
      type = llvm::Type::getIntNTy(*TheContext, 256);
    }
    memberTypes.push_back(type);
  }
  selfType = llvm::StructType::create(*TheContext, memberTypes, "self_type");
  llvm::Constant *init = llvm::Constant::getNullValue(selfType);
  self = new llvm::GlobalVariable(
      *TheModule, selfType, false,
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, init, "__self");
}

llvm::Type *LLVMCodegenVisitor::getTypeByBitwidth(int bitWidth) {
  /**
   * @todo fix this for other types
   *
   */
  assert(bitWidth == 256);
  return llvm::Type::getIntNTy(*TheContext, bitWidth);
}

void LLVMCodegenVisitor::dump(llvm::raw_ostream &os) const {
  TheModule->print(os, nullptr);
}

void LLVMCodegenVisitor::dumpToStdout() const { dump(llvm::outs()); }

llvm::GlobalVariable *LLVMCodegenVisitor::getSelf() const {
  assert(self && "Self is accessed but not built yet");
  return self;
}
llvm::StructType *LLVMCodegenVisitor::getSelfType() const {
  assert(self && "SelfTypeis accessed but not built yet");
  return selfType;
}

llvm::legacy::FunctionPassManager &LLVMCodegenVisitor::getFPM() { return *FPM; }