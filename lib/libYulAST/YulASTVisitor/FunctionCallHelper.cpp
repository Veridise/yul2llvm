#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/FunctionCallHelper.h>
YulFunctionCallHelper::YulFunctionCallHelper(LLVMCodegenVisitor &v)
    : visitor(v), intrinsicEmitter(v) {}

llvm::Type *YulFunctionCallHelper::getReturnType(YulFunctionCallNode &node) {

  if (!node.getCalleeName().compare("revert"))
    return llvm::Type::getVoidTy(visitor.getContext());
  return llvm::Type::getIntNTy(visitor.getContext(), 256);
}

std::vector<llvm::Type *>
YulFunctionCallHelper::getFunctionArgTypes(YulFunctionCallNode &node) {
  int numargs = node.getArgs().size();
  std::vector<llvm::Type *> funcArgTypes(
      numargs, llvm::Type::getIntNTy(visitor.getContext(), 256));
  return funcArgTypes;
}

llvm::Function *YulFunctionCallHelper::createPrototype(
    YulFunctionCallNode &node,
    llvm::SmallVector<llvm::Attribute::AttrKind> &attrs) {
  std::vector<llvm::Type *> funcArgTypes = getFunctionArgTypes(node);
  llvm::Type *retType = getReturnType(node);
  llvm::FunctionType *FT =
      llvm::FunctionType::get(retType, funcArgTypes, false);

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
  F = visitor.getModule().getFunction(node.getCalleeName());
  if (!F) {
    auto attrList = buildFunctionAttributes(node);
    F = createPrototype(node, *attrList);
  }
  assert(F && "Function not found and could not be created");
  std::vector<llvm::Value *> ArgsV;

  for (auto &a : node.getArgs()) {
    llvm::Value *lv = visitor.visit(*a);
    ArgsV.push_back(lv);
  }
  // std::cout<<"Creating call "<<callee->getIdentfierValue()<<std::endl;
  if (F->getReturnType() == llvm::Type::getVoidTy(visitor.getContext())) {
    visitor.getBuilder().CreateCall(F, ArgsV);
    return nullptr;
  } else {
    return visitor.getBuilder().CreateCall(F, ArgsV, node.getCalleeName());
  }
}