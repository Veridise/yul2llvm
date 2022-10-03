#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/FunctionCallHelper.h>
YulFunctionCallHelper::YulFunctionCallHelper(LLVMCodegenVisitor &v,
                                             YulIntrinsicHelper &ih)
    : visitor(v), intrinsicEmitter(ih) {}

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
  llvm::Function *F;
  if (intrinsicEmitter.isFunctionCallIntrinsic(node.getCalleeName())) {
    return intrinsicEmitter.handleIntrinsicFunctionCall(node);
  }
  llvm::SmallVector<llvm::Value *> ArgsV;
  auto functionIt = visitor.definedFunctions.find(node.getCalleeName());
  if (functionIt != visitor.definedFunctions.end()) {
    // Add self only to functions that are defined in this contract,
    // not to functions representing evm opcodes and external functions
    ArgsV.push_back(visitor.getSelfArg());
  }
  for (auto &a : node.getArgs()) {
    llvm::Value *lv = visitor.visit(*a);
    assert(lv != nullptr && "null value argument to a function call");
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