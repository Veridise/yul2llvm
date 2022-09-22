#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicHelper.h>

bool YulIntrinsicHelper::isFunctionCallIntrinsic(std::string calleeName) {
  if (calleeName == "checked_add_t_uint256") {
    return true;
  } else if (calleeName == "mstore") {
    return true;
  } else if (calleeName == "add") {
    return true;
  } else if (calleeName == "shl") {
    return true;
  }
  return false;
}

llvm::Value *
YulIntrinsicHelper::handleIntrinsicFunctionCall(YulFunctionCallNode &node) {
  std::string calleeName = node.getCalleeName();
  if (!calleeName.compare("checked_add_t_uint256")) {
    return handleAddFunctionCall(node);
  } else if (!calleeName.compare("mstore")) {
    return handleMStoreFunctionCall(node);
  } else if (!calleeName.compare("add")) {
    return handleAddFunctionCall(node);
  } else if (!calleeName.compare("shl")) {
    return handleShl(node);
  }
  return nullptr;
}

llvm::Value *YulIntrinsicHelper::handleShl(YulFunctionCallNode &node) {
  auto &builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  return builder.CreateShl(v1, v2);
}

llvm::Value *
YulIntrinsicHelper::handleAddFunctionCall(YulFunctionCallNode &node) {
  llvm::IRBuilder<> &Builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  return Builder.CreateAdd(v1, v2);
}

llvm::Value *
YulIntrinsicHelper::handleMStoreFunctionCall(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 &&
         "Incorrect number of arguments to mstore");
  llvm::Value *val, *ptr;
  ptr = visitor.visit(*(node.getArgs()[0]));
  val = visitor.visit(*(node.getArgs()[1]));
  /**
   *  @todo: This pointer cast may not be correct check again
   */
  if (!ptr->getType()->isPointerTy())
    ptr = visitor.getBuilder().CreateIntToPtr(ptr,
                                              val->getType()->getPointerTo());
  visitor.getBuilder().CreateStore(val, ptr);
  return nullptr;
}
