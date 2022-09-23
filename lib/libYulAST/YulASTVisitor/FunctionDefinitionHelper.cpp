#include <iostream>
#include <libYulAST/YulASTVisitor/FunctionDefinitionHelper.h>

YulFunctionDefinitionHelper::YulFunctionDefinitionHelper(LLVMCodegenVisitor &v)
    : visitor(v), intrinsicEmitter(v) {}

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

  for (auto &f : F->args()) {
    visitor.getBuilder().CreateStore(
        &f, visitor.getNamedValuesMap()[f.getName().str()]);
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
  F = visitor.getModule().getFunction(node.getName());
  assert(F && "Function not defined in declarator pass");
  visitor.currentFunction = F;
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
  visitor.currentFunction = nullptr;
}
