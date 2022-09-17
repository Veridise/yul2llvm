#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/FunctionCallHelper.h>
YulFunctionCallHelper::YulFunctionCallHelper(LLVMCodegenVisitor &v)
    : visitor(v), intrinsicEmitter(v) {}

llvm::Function *YulFunctionCallHelper::createPrototype(
    YulFunctionCallNode &node,
    llvm::SmallVector<llvm::Attribute::AttrKind> &attrs,
    llvm::SmallVector<llvm::Value *> &argsV) {
  llvm::FunctionType *FT = intrinsicEmitter.getFunctionType(node, argsV);

  llvm::Function *F =
      llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             node.getCalleeName(), visitor.getModule());
  for (auto &att : attrs) {
    F->addAttribute(0, att);
  }
  return F;
}

std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>>
YulFunctionCallHelper::buildFunctionAttributes(YulFunctionCallNode &node) {
  auto attributes =
      std::make_unique<llvm::SmallVector<llvm::Attribute::AttrKind>>();
  if (node.getCalleeName().substr(0, 7) == "revert_") {
    // attributes->push_back(llvm::Attribute::NoReturn);
  }
  return attributes;
}

llvm::Value *
YulFunctionCallHelper::visitYulFunctionCallNode(YulFunctionCallNode &node) {
  llvm::Function *F, *enclosingFunction;
  enclosingFunction = visitor.getBuilder().GetInsertBlock()->getParent();
  if (intrinsicEmitter.isFunctionCallIntrinsic(node.getCalleeName())) {
    return intrinsicEmitter.handleIntrinsicFunctionCall(node);
  }
  llvm::SmallVector<llvm::Value *> ArgsV;

  for (auto &a : node.getArgs()) {
    llvm::Value *lv = visitor.visit(*a);
    ArgsV.push_back(lv);
  }
  F = visitor.getModule().getFunction(node.getCalleeName());
  if (!F) {
    auto attrList = buildFunctionAttributes(node);
    F = createPrototype(node, *attrList, ArgsV);
  }
  assert(F && "Function not found and could not be created");

  // std::cout<<"Creating call "<<callee->getIdentfierValue()<<std::endl;
  if (F->getReturnType() == llvm::Type::getVoidTy(visitor.getContext())) {
    visitor.getBuilder().CreateCall(F, ArgsV);
    return nullptr;
  } else {
    return visitor.getBuilder().CreateCall(F, ArgsV, node.getCalleeName());
  }
}