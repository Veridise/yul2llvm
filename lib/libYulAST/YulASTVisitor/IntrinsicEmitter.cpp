#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicEmitter.h>

YulIntrinsicEmitter::YulIntrinsicEmitter(LLVMCodegenVisitor &v) : visitor(v) {}
bool YulIntrinsicEmitter::isFunctionCallIntrinsic(std::string calleeName) {
  if (calleeName == "pyul_storage_var_load") {
    return true;
  } else if (calleeName == "pyul_storage_var_update") {
    return true;
  } else if (!calleeName.compare("checked_add_t_uint256")) {
    return true;
  }
  return false;
}

llvm::Value *
YulIntrinsicEmitter::handleIntrinsicFunctionCall(YulFunctionCallNode &node) {
  std::string calleeName = node.getCalleeName();
  if (calleeName == "pyul_storage_var_load") {
    return emitStorageLoadIntrinsic(node);
  } else if (calleeName == "pyul_storage_var_update") {
    emitStorageStoreIntrinsic(node);
    return nullptr;
  } else if (!calleeName.compare("checked_add_t_uint256")) {
    return handleAddFunctionCall(node);
  }
  return nullptr;
}

llvm::Value *
YulIntrinsicEmitter::handleAddFunctionCall(YulFunctionCallNode &node) {
  llvm::IRBuilder<> &Builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  return Builder.CreateAdd(v1, v2);
}

llvm::Value *
YulIntrinsicEmitter::emitStorageLoadIntrinsic(YulFunctionCallNode &node) {
  auto &args = node.getArgs();
  assert(args.size() == 2);
  auto structFieldOrder = visitor.currentContract->getStructFieldOrder();
  auto typeMap = visitor.currentContract->getTypeMap();
  assert(args[0]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  assert(args[1]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  YulLiteralNode &lit0 = (YulLiteralNode &)(*(args[0]));
  YulLiteralNode &lit1 = (YulLiteralNode &)(*(args[1]));
  assert(lit0.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  assert(lit1.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  YulStringLiteralNode &varLit = (YulStringLiteralNode &)(*(args[0]));
  auto fieldIt = std::find(structFieldOrder.begin(), structFieldOrder.end(),
                           varLit.to_string());
  assert(fieldIt != structFieldOrder.end());
  int structIndex = fieldIt - structFieldOrder.begin();
  llvm::SmallVector<llvm::Value *> indices;
  int bitWidth = std::get<1>(typeMap[*fieldIt]);
  indices.push_back(
      llvm::ConstantInt::get(visitor.getContext(), llvm::APInt(32, 0, false)));
  indices.push_back(llvm::ConstantInt::get(
      visitor.getContext(), llvm::APInt(32, structIndex, false)));
  llvm::Value *ptr = visitor.getBuilder().CreateGEP(
      visitor.getSelfType(), (llvm::Value *)visitor.getSelf(), indices,
      "ptr_self_" + varLit.to_string());
  return visitor.getBuilder().CreateLoad(
      llvm::Type::getIntNTy(visitor.getContext(), bitWidth), ptr,
      "self_" + varLit.to_string());
}

void YulIntrinsicEmitter::emitStorageStoreIntrinsic(YulFunctionCallNode &node) {
  auto &args = node.getArgs();
  assert(args.size() == 3);
  auto structFieldOrder = visitor.currentContract->getStructFieldOrder();
  auto typeMap = visitor.currentContract->getTypeMap();
  assert(args[0]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  assert(args[1]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_IDENTIFIER);
  assert(args[2]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  YulLiteralNode &name = (YulLiteralNode &)(*(args[0]));
  assert(name.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  YulStringLiteralNode &varLit = (YulStringLiteralNode &)(*(args[0]));
  YulExpressionNode &valueNode = *(args[1]);
  auto fieldIt = std::find(structFieldOrder.begin(), structFieldOrder.end(),
                           varLit.to_string());
  assert(fieldIt != structFieldOrder.end());
  int structIndex = fieldIt - structFieldOrder.begin();
  llvm::SmallVector<llvm::Value *> indices;
  indices.push_back(
      llvm::ConstantInt::get(visitor.getContext(), llvm::APInt(32, 0, false)));
  indices.push_back(llvm::ConstantInt::get(
      visitor.getContext(), llvm::APInt(32, structIndex, false)));
  llvm::Value *ptr = visitor.getBuilder().CreateGEP(
      visitor.getSelfType(), (llvm::Value *)visitor.getSelf(), indices,
      "ptr_self_" + varLit.to_string());
  llvm::Value *storeValue = visitor.visit(valueNode);
  /**
   * llvm::Type *loadType = llvm::Type::getIntNTy(*TheContext, 256);
   * @todo fix all bit widths;
   */
  visitor.getBuilder().CreateStore(storeValue, ptr, false);
}