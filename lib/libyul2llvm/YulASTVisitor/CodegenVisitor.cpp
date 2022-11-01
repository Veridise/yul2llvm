#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>
using namespace yulast;

// helpers

llvm::StructType *constructExtCallCtxType(llvm::LLVMContext &TheContext) {
  llvm::Type *int256Type = llvm::Type::getIntNTy(TheContext, 256);
  llvm::Type *int256PtrType = llvm::Type::getIntNPtrTy(TheContext, 256);
  llvm::SmallVector<llvm::Type *> types = {
      int256Type, // gas
      int256Type, // addr
      int256Type, // value
      int256PtrType,
      int256Type // ret len success
  };
  return llvm::StructType::create(TheContext, types, "ExtCallContextType");
}

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

void LLVMCodegenVisitor::connectToBasicBlock(llvm::BasicBlock *nextBlock) {
  llvm::BasicBlock *lastBlock = Builder->GetInsertBlock();
  if (lastBlock->getInstList().size() == 0 ||
      !isUnconditionalTerminator(&(lastBlock->getInstList().back()))) {
    Builder->CreateBr(nextBlock);
  }
}

LLVMCodegenVisitor::LLVMCodegenVisitor() : intrinsicHelper(*this) {
  // LLVM functions and datastructures
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("yul", *TheContext);
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
  extCallCtxType = constructExtCallCtxType(*TheContext);
  funCallHelper =
      std::make_unique<YulFunctionCallHelper>(*this, intrinsicHelper);
  funDefHelper =
      std::make_unique<YulFunctionDefinitionHelper>(*this, intrinsicHelper);
  FPM = std::make_unique<llvm::legacy::FunctionPassManager>(TheModule.get());
  FPM->add(llvm::createLoopSimplifyPass());
  FPM->add(llvm::createPromoteMemoryToRegisterPass());
  //@todo add dce
}

void LLVMCodegenVisitor::runFunctionDeclaratorVisitor(YulContractNode &node) {
  FunctionDeclaratorVisitor declVisitor(*TheContext, *TheModule,
                                        intrinsicHelper);
  declVisitor.visit(node);
}

llvm::AllocaInst *LLVMCodegenVisitor::CreateEntryBlockAlloca(
    llvm::Function *TheFunction, const std::string &VarName, llvm::Type *type) {

  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin());
  if (!type)
    type = llvm::Type::getIntNTy(*TheContext, 256);
  return TmpB.CreateAlloca(type, 0, VarName);
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
  if (node.getLHSIdentifiers().size() == 1) {
    auto &id = node.getLHSIdentifiers()[0];
    // CASE: Assignment to one variable with initilizer value
    llvm::Value *initValue = visit(node.getRHSExpression());
    llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
    Builder->CreateStore(initValue, lval);
  } else {
    // CASE: Assignment of multiple value (tuple assignment)
    YulExpressionNode &rhsExpression = node.getRHSExpression();
    if (rhsExpression.expressionType ==
        YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_FUNCTION_CALL) {
      // CASE: RHS is a function call.
      auto rets = unpackFunctionCallReturns(rhsExpression);
      llvm::Value *initValue;
      // Make sure number of variables in LHS == number of values returned by
      // function.
      assert(node.getLHSIdentifiers().size() == rets.size() &&
             "Number of vars and returns mismatch");
      int idx = 0;
      for (auto &id : node.getLHSIdentifiers()) {
        initValue = rets[idx];
        llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
        Builder->CreateStore(initValue, lval);
        idx++;
      }
    } else {
      //@todo raise runtime error
      assert(false && "unhandled rhs in assignment");
    }
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
  constructSelfStructType(node);
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
  llvm::AllocaInst *ptr;
  if (NamedValues.find(node.getIdentfierValue()) == NamedValues.end()) {
    for (auto &arg : currentFunction->args()) {
      if (!std::string(arg.getName()).compare(node.getIdentfierValue())) {
        return &arg;
      }
    }
  } else {
    llvm::Type *loadType = llvm::Type::getIntNTy(*TheContext, 256);
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
  /**
   * @brief if last instruction is an uncoditional terminator, add a jump from
   * then-body to join node(not-taken-body)
   *
   */
  connectToBasicBlock(contBlock);
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
  if (auto it = stringLiteralNames.find(node.getLiteralValue());
      it != stringLiteralNames.end()) {
    std::string globalName = it->getValue();
    return TheModule->getOrInsertGlobal(globalName,
                                        llvm::Type::getInt8Ty(*TheContext));
  }
  return CreateGlobalStringLiteral(node.getLiteralValue(), literalName);
}
void LLVMCodegenVisitor::visitYulSwitchNode(YulSwitchNode &node) {
  llvm::Value *cond = visit(node.getCondition());
  llvm::BasicBlock *defaultBlock =
      llvm::BasicBlock::Create(*TheContext, "default");
  llvm::BasicBlock *cont = llvm::BasicBlock::Create(*TheContext, "switch-cont");

  int numTargets = node.getCases().size() + (node.hasDefaultNode() ? 1 : 0);

  llvm::SwitchInst *sw;
  if (node.hasDefaultNode())
    sw = Builder->CreateSwitch(cond, defaultBlock, numTargets);
  else
    sw = Builder->CreateSwitch(cond, cont, numTargets);

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
    connectToBasicBlock(cont);
  }

  if (node.hasDefaultNode()) {
    currentFunction->getBasicBlockList().push_back(defaultBlock);
    Builder->SetInsertPoint(defaultBlock);
    visit(node.getDefaultNode());
    connectToBasicBlock(cont);
  }
  currentFunction->getBasicBlockList().push_back(cont);
  Builder->SetInsertPoint(cont);
}
void LLVMCodegenVisitor::visitYulVariableDeclarationNode(
    YulVariableDeclarationNode &node) {
  if (node.getVars().size() == 1) {
    // CASE: Declaring one variable
    auto &id = node.getVars()[0];
    if (node.hasValue()) {
      // CASE: Declaring one variable with initilizer value
      llvm::Value *initValue = visit(node.getValue());
      codeGenForOneVarAllocation(*id, initValue->getType());
      llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
      Builder->CreateStore(initValue, lval);
    } else {
      // CASE: Declaring one variable without intialial value
      codeGenForOneVarAllocation(*id, nullptr);
    }
  } else {
    // CASE: Multiple variables declared in one decl statement
    if (node.hasValue()) {
      // CASE: Multiple variables declared in one decl statement initialized
      // with a node that returns
      // multiple values.
      YulExpressionNode &rhsExpression = node.getValue();
      if (rhsExpression.expressionType ==
          YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_FUNCTION_CALL) {
        // CASE: Initilizer is a function call.
        llvm::Value *initValue;
        // Make sure number of variables declared == number of values returned
        // by function.
        auto rets = unpackFunctionCallReturns(rhsExpression);
        assert(node.getVars().size() == rets.size() &&
               "Number of vars and returns mismatch");
        int idx = 0;
        for (auto &id : node.getVars()) {
          initValue = rets[idx];
          codeGenForOneVarAllocation(*id, initValue->getType());
          llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
          Builder->CreateStore(initValue, lval);
          idx++;
        }
      } else {
        //@todo raise runtime error
        assert(false && "unhandled rhs in var decl");
      }
    } else {
      // CASE: Muiltiple variabled declared without an initializer
      for (auto &id : node.getVars()) {
        codeGenForOneVarAllocation(*id, nullptr);
      }
    }
  }
}
void LLVMCodegenVisitor::codeGenForOneVarAllocation(YulIdentifierNode &id,
                                                    llvm::Type *type) {
  if (NamedValues[id.getIdentfierValue()] == nullptr) {
    llvm::AllocaInst *v =
        CreateEntryBlockAlloca(currentFunction, id.getIdentfierValue(), type);
    NamedValues[id.getIdentfierValue()] = v;
  }
}

void LLVMCodegenVisitor::constructSelfStructType(YulContractNode &node) {
  if (node.getStructFieldOrder().size() == 0)
    return;
  std::vector<llvm::Type *> memberTypes;
  auto &typeInfoMap = node.getTypeInfoMap();
  for (auto &field : node.getStructFieldOrder()) {
    std::string typeStr = node.getVarTypeMap()[field].type;
    llvm::Type *type = getTypeByInfo(typeStr, typeInfoMap);
    memberTypes.push_back(type);
  }
  selfType = llvm::StructType::create(*TheContext, memberTypes, "self_type");
}

llvm::Type *LLVMCodegenVisitor::getTypeByInfo(
    llvm::StringRef typeStr, std::map<std::string, TypeInfo> &typeInfoMap) {
  llvm::Type *memPtrType = llvm::Type::getIntNPtrTy(*TheContext, 256);
  if (typeStr.startswith("t_mapping")) {
    return memPtrType;
  } else if (typeStr.find("t_array") != typeStr.npos) {
    return memPtrType;
  } else {
    int bitwidth = typeInfoMap[typeStr.str()].size * 8;
    return llvm::Type::getIntNTy(*TheContext, bitwidth);
  }
}

llvm::SmallVector<llvm::Value *>
LLVMCodegenVisitor::getLLVMValueVector(llvm::ArrayRef<int> rawIndices) {
  llvm::SmallVector<llvm::Value *> indices;
  for (int i : rawIndices) {
    indices.push_back(
        llvm::ConstantInt::get(*TheContext, llvm::APInt(32, i, false)));
  }
  return indices;
}

llvm::StructType *LLVMCodegenVisitor::getExtCallCtxType() {
  return extCallCtxType;
}

llvm::Value *LLVMCodegenVisitor::getSelfArg() const {
  assert(currentFunction && "currentFunction null while getting silfArg");
  return currentFunction->arg_begin();
}
void LLVMCodegenVisitor::dump(llvm::raw_ostream &os) const {
  TheModule->print(os, nullptr);
}

void LLVMCodegenVisitor::dumpToStdout() const { dump(llvm::outs()); }

llvm::StructType *LLVMCodegenVisitor::getSelfType() const {
  assert(selfType && "SelfType is accessed but not built yet");
  return selfType;
}

llvm::Type *LLVMCodegenVisitor::getDefaultType() const {
  return llvm::Type::getIntNTy(*TheContext, 256);
}

llvm::StringMap<llvm::StructType *> &LLVMCodegenVisitor::getReturnTypesMap() {
  return returnTypes;
}
llvm::StringMap<llvm::Value *> &LLVMCodegenVisitor::getReturnStructs() {
  return returnStructs;
}

llvm::Value *
LLVMCodegenVisitor::packRetsInStruct(llvm::StringRef functionName,
                                     llvm::ArrayRef<llvm::Value *> rets,
                                     llvm::Instruction *callInst) {
  llvm::IRBuilder<> builder(callInst);
  llvm::StructType *retType = returnTypes[functionName];
  llvm::Value *retStructPtr = returnStructs[functionName];
  int idx = 0;
  for (llvm::Value *v : rets) {
    llvm::Value *elementPtr =
        builder.CreateGEP(retType, retStructPtr, getLLVMValueVector({0, idx}));
    builder.CreateStore(v, elementPtr);
    idx++;
  }
  return retStructPtr;
}

llvm::SmallVector<llvm::Value *> LLVMCodegenVisitor::unpackFunctionCallReturns(
    YulExpressionNode &rhsExpression) {
  YulFunctionCallNode &callNode =
      static_cast<YulFunctionCallNode &>(rhsExpression);
  std::string functionName = callNode.getCalleeName();
  auto typeIt = returnTypes.find(functionName);
  assert(typeIt != returnTypes.end() && "could not find return type to unpack");
  llvm::StructType *retType = typeIt->getValue();
  llvm::Value *retStructPtr = visit(callNode);
  llvm::SmallVector<llvm::Value *> rets;
  for (unsigned int idx = 0; idx < retType->getNumElements(); idx++) {
    llvm::Value *retElemPtr =
        Builder->CreateGEP((llvm::Type *)retType, retStructPtr,
                           getLLVMValueVector({0, static_cast<int>(idx)}));
    llvm::Value *initValue =
        Builder->CreateLoad(retType->getElementType(idx), retElemPtr);
    rets.push_back(initValue);
  }
  return rets;
}

YulIntrinsicHelper &LLVMCodegenVisitor::getYulIntrisicHelper() {
  return intrinsicHelper;
}

llvm::legacy::FunctionPassManager &LLVMCodegenVisitor::getFPM() { return *FPM; }
