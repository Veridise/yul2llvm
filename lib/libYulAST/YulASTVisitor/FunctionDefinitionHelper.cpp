#include <iostream>
#include <libYulAST/YulASTVisitor/FunctionDefinitionHelper.h>

YulFunctionDefinitionHelper::YulFunctionDefinitionHelper(LLVMCodegenVisitor &v,
                                                         YulIntrinsicHelper &ih)
    : visitor(v), intrinsicEmitter(ih) {}

void YulFunctionDefinitionHelper::createVarsForArgsAndRets(
    YulFunctionDefinitionNode &node, llvm::Function *F) {
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(visitor.getContext(), "entry", F);
  visitor.getBuilder().SetInsertPoint(BB);

  if (node.hasArgs()) {
    for (auto &arg : node.getArgs()) {
      llvm::AllocaInst *a =
          visitor.CreateEntryBlockAlloca(F, arg->getIdentfierValue());
      visitor.getNamedValuesMap()[arg->getIdentfierValue()] = a;
    }
  }

  bool selfArgVisited = false;
  for (auto &f : F->args()) {
    if (!selfArgVisited) {
      selfArgVisited = true; // skip self arg
    } else {
      visitor.getBuilder().CreateStore(
          &f, visitor.getNamedValuesMap()[f.getName().str()]);
    }
  }

  if (node.hasRets()) {
    for (auto &arg : node.getRets()) {
      llvm::AllocaInst *a =
          visitor.CreateEntryBlockAlloca(F, arg->getIdentfierValue());
      visitor.getNamedValuesMap()[arg->getIdentfierValue()] = a;
    }
  }
}

void YulFunctionDefinitionHelper::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode &node) {
  llvm::Function *F;
  if (intrinsicEmitter.skipDefinition(node.getName())) {
    return;
  }
  F = visitor.getModule().getFunction(node.getName());
  assert(F && "Function not defined in declarator pass");
  visitor.currentFunction = F;
  if (F->getBasicBlockList().size() > 0) {
    llvm::WithColor::error() << "Redeclaring function " << F->getName();
    return;
  }
  visitor.getNamedValuesMap().clear();
  createVarsForArgsAndRets(node, F);
  visitor.visit(node.getBody());
  if (!node.hasRets()) {
    visitor.getBuilder().CreateRetVoid();
  } else {
    // @todo assuming rets has only a single element
    llvm::Value *v = visitor.getBuilder().CreateLoad(
        llvm::Type::getIntNTy(visitor.getContext(), 256),
        visitor.getNamedValuesMap()[node.getRets()[0]->getIdentfierValue()]);
    visitor.getBuilder().CreateRet(v);
  }
  visitor.getFPM().run(*F);
  intrinsicEmitter.rewriteIntrinsics(F);
  visitor.getFPM().run(*F);
  visitor.currentFunction = nullptr;
}
