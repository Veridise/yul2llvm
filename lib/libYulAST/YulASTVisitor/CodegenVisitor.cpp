#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
using namespace yulast;

LLVMCodegenVisitor::LLVMCodegenVisitor():funCallHelper(*this), funDefHelper(*this){
  // LLVM functions and datastructures
  TheContext =
      std::make_unique<llvm::LLVMContext>();
  TheModule =
      std::make_unique<llvm::Module>("yul", *TheContext);
  Builder =
      std::make_unique<llvm::IRBuilder<>>(*TheContext);
  
}

llvm::AllocaInst *LLVMCodegenVisitor::CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                   const std::string &VarName) {
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(llvm::Type::getIntNTy(*TheContext, 256), 0,
                           VarName.c_str());
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

//visitors

void LLVMCodegenVisitor::visitYulAssignmentNode(YulAssignmentNode &node) {
  for (auto &var : node.getLHSIdentifiers()) {
    std::string lvalname = var->getIdentfierValue();
    llvm::AllocaInst *lval = NamedValues[lvalname];
    if (lval == nullptr) {
      llvm::WithColor::error()<< "undefined variable " << lvalname;
      exit(1);
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
  assert(loopControlFlowBlocks.size()>0 && "Break not in loop");
  llvm::BasicBlock *contBB = std::get<0>(loopControlFlowBlocks.top());
  Builder->CreateBr(contBB);
}

void LLVMCodegenVisitor::visitYulCaseNode(YulCaseNode &node) {
  visit(node.getThenBody());
  
}
void LLVMCodegenVisitor::visitYulContinueNode(YulContinueNode &node) {
  assert(loopControlFlowBlocks.size()>0 && "Break not in loop");
  llvm::BasicBlock *contBB = std::get<1>(loopControlFlowBlocks.top());
  Builder->CreateBr(contBB);
}
void LLVMCodegenVisitor::visitYulContractNode(YulContractNode &node) {
  constructStruct(node);
  for (auto &f : node.getFunctions()) {
    f->codegen(nullptr);
  }

  
}

void LLVMCodegenVisitor::visitYulDefaultNode(YulDefaultNode &node) {
  visit(node.getThenBody());
}

void LLVMCodegenVisitor::visitYulForNode(YulForNode &node) {
  llvm::Function *enclosingFunction = Builder->GetInsertBlock()->getParent();
  // initializetion code gen can happen in current basic block
  visit(node.getInitializationNode());
  // create termination condtition basic block
  llvm::BasicBlock *condBB = llvm::BasicBlock::Create(
      *TheContext, "loop-cond", enclosingFunction);
  llvm::BasicBlock *contBB = llvm::BasicBlock::Create(
      *TheContext, "for-cont", enclosingFunction);
  llvm::BasicBlock *incrBB = llvm::BasicBlock::Create(
      *TheContext, "for-incr", enclosingFunction);
  

  Builder->SetInsertPoint(condBB);
  visit(node.getConditionNode());

  // create body basic block
  Builder->GetInsertBlock()->setName("for-body");
  loopControlFlowBlocks.push(std::make_tuple(contBB, incrBB));
  visit(node.getBody());
  loopControlFlowBlocks.pop();
  
  // create increment basic block,
  // creating a separate basic block because cont can jump here
  Builder->SetInsertPoint(incrBB);
  visit(node.getIncrementNode());
  Builder->CreateBr(condBB);

  // Reorder the cont basic block
  enclosingFunction->getBasicBlockList().remove(contBB);
  enclosingFunction->getBasicBlockList().push_back(contBB);

  Builder->SetInsertPoint(contBB);
}

llvm::Value *LLVMCodegenVisitor::visitYulFunctionCallNode(YulFunctionCallNode &node){
  funCallHelper.visitYulFunctionCallNode(node);
}


void LLVMCodegenVisitor::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode &node) {
  funDefHelper.visitYulFunctionDefinitionNode(node);
}

llvm::Value *
LLVMCodegenVisitor::visitYulIdentifierNode(YulIdentifierNode &node) {
  llvm::Type *inttype = llvm::Type::getIntNTy(*TheContext, 256);
  if (NamedValues.find(node.getIdentfierValue()) == NamedValues.end()) {
    for (auto &arg : currentFunction->args()) {
      if (!std::string(arg.getName()).compare(node.getIdentfierValue())) {
        return &arg;
      }
    }
  }
  return Builder->CreateLoad(inttype, NamedValues[node.getIdentfierValue()],
                             node.getIdentfierValue());
}
void LLVMCodegenVisitor::visitYulIfNode(YulIfNode &node) {
  llvm::BasicBlock *thenBlock = llvm::BasicBlock::Create(
      *TheContext, "if-taken-body", currentFunction);
  llvm::BasicBlock *contBlock = llvm::BasicBlock::Create(
      *TheContext, "if-not-taken-body");
  llvm::Value *cond = visit(node.getCondition());
  cond = Builder->CreateCast(llvm::Instruction::CastOps::Trunc, cond,
                             llvm::IntegerType::getInt1Ty(*TheContext));

  // create actual branch on condition
  Builder->CreateCondBr(cond, thenBlock, contBlock);

  // emit then
  Builder->SetInsertPoint(thenBlock);
  visit(node.getThenBody());
  Builder->CreateBr(contBlock);

  // merge node
  currentFunction->getBasicBlockList().push_back(contBlock);
  Builder->SetInsertPoint(contBlock);  
}
void LLVMCodegenVisitor::visitYulLeaveNode(YulLeaveNode &node) {
  
}
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
  if (auto x =
          stringLiteralNames.find(node.getLiteralValue()) != stringLiteralNames.end()) {
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
        *TheContext,
        c->getCondition()->to_string() + "-case",
        currentFunction);
    llvm::APInt &literal = c->getCondition()->getLiteralValue();
    sw->addCase(llvm::ConstantInt::get(*TheContext, literal), caseBB);
    Builder->SetInsertPoint(caseBB);
    visit(*c);
    Builder->CreateBr(cont);
  }

  currentFunction->getBasicBlockList().push_back(defaultBlock);
  Builder->SetInsertPoint(defaultBlock);
  visit(node.getDefaultNode());
  Builder->CreateBr(cont);
  currentFunction->getBasicBlockList().push_back(cont);
  Builder->SetInsertPoint(cont);
}
void LLVMCodegenVisitor::visitYulVariableDeclarationNode(
    YulVariableDeclarationNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulVariableDeclarationNode codegen not implemented";
}