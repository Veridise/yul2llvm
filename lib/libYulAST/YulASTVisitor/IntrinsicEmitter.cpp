#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicEmitter.h>

YulIntrinsicEmitter::YulIntrinsicEmitter(LLVMCodegenVisitor &v) : visitor(v) {}
bool YulIntrinsicEmitter::isFunctionCallIntrinsic(std::string calleeName) {
  if (calleeName == "pyul_storage_var_load") {
    return true;
  } else if (calleeName == "pyul_storage_var_update") {
    return true;
  } else if (calleeName == "checked_add_t_uint256") {
    return true;
  } else if (calleeName == "__pyul_map_index") {
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
  } else if (!calleeName.compare("__pyul_map_index")) {
    return handleMapIndex(node);
  }
  return nullptr;
}
/**
 * @brief This function is a first pass of 2pass approach for translating
 * mapping_index_* function The approach is as follows:
 *   1. Rewrite mapping_index_* function to __pyul_map_index in python
 *   2. Emit __pyul_map_index(expr, expr) function
 *   3. Run optimization pass. Optimization will replace expr with constants
 * wherever possible.
 *   4. Rewrite all __pyul_map_index to pyul_map_index, where
 *      if first arg to __pyul_map_index is constant, use this offset to find
 * the base address of map in self struct.
 *
 *   This works because, the second index calculation (second pyul_map_index)
 * will never have constant first arg
 *
 * @param node
 * @return llvm::Value*
 */
llvm::Value *YulIntrinsicEmitter::handleMapIndex(YulFunctionCallNode &node) {
  llvm::Function *f = visitor.getModule().getFunction("__pyul_map_index");
  llvm::Type *ptrType = llvm::Type::getIntNPtrTy(visitor.getContext(), 256);
  if (!f) {
    llvm::SmallVector<llvm::Type *> argtype;
    argtype.push_back(ptrType);
    argtype.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
    llvm::FunctionType *ft = llvm::FunctionType::get(ptrType, argtype, false);
    f = llvm::Function::Create(ft,
                               llvm::Function::LinkageTypes::ExternalLinkage,
                               "__pyul_map_index", visitor.getModule());
  }
  llvm::SmallVector<llvm::Value *> args;
  for (auto &arg : node.getArgs()) {
    llvm::Value *v = visitor.visit(*arg);
    args.push_back(v);
  }

  return visitor.getBuilder().CreateCall(f, args, "__pyul_map_index");
}

llvm::Value *
YulIntrinsicEmitter::handleAddFunctionCall(YulFunctionCallNode &node) {
  llvm::IRBuilder<> &Builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  return Builder.CreateAdd(v1, v2);
}

llvm::FunctionType *YulIntrinsicEmitter::getMapIndexFT() {
  llvm::SmallVector<llvm::Type *> args;
  llvm::Type *retType = llvm::Type::getIntNPtrTy(visitor.getContext(), 256);
  args.push_back(retType);
  args.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
  llvm::FunctionType *FT = llvm::FunctionType::get(retType, args, false);
  return FT;
}

llvm::Function *
YulIntrinsicEmitter::getOrCreateFunction(std::string name,
                                         llvm::FunctionType *FT) {
  llvm::Function *F = nullptr;
  F = visitor.getModule().getFunction(name);
  if (F)
    return F;
  else {
    F = llvm::Function::Create(FT,
                               llvm::Function::LinkageTypes::ExternalLinkage,
                               name, visitor.getModule());
    return F;
  }
}

void printFunction(llvm::Function *enclosingFunction) {
  enclosingFunction->print(llvm::outs());
  llvm::outs() << "\n";
}

void YulIntrinsicEmitter::rewriteMapIndexCalls(
    llvm::Function *enclosingFunction) {
  llvm::SmallVector<llvm::CallInst *> oldInstructions;
  for (auto b = enclosingFunction->begin(); b != enclosingFunction->end();
       b++) {
    for (auto i = b->begin(); i != b->end(); i++) {
      auto inst = llvm::dyn_cast<llvm::CallInst>(&(*i));
      if (inst) {
        if (inst->getCalledFunction()->getName() == "__pyul_map_index") {
          oldInstructions.push_back(inst);
        }
      }
    }
  }
  llvm::FunctionType *FT = getMapIndexFT();
  llvm::Function *mapIndexFunction = getOrCreateFunction("pyul_map_index", FT);
  for (auto &callInst : oldInstructions) {
    auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
    if (offset) {
      std::string varname =
          visitor.currentContract->getStateVarNameByOffsetSlot(
              offset->getZExtValue(), 0);
      llvm::Value *mapPtr = getPointerToStorageVarByName(varname);
      llvm::Value *key = callInst->getArgOperand(1);
      llvm::SmallVector<llvm::Value *> args;
      args.push_back(mapPtr);
      args.push_back(key);
      llvm::CallInst *newCall =
          llvm::CallInst::Create(mapIndexFunction, args, "pyul_map_index");
      llvm::ReplaceInstWithInst(callInst, newCall);
    } else {
      llvm::SmallVector<llvm::Value *> args;
      for (auto &arg : callInst->args()) {
        args.push_back(arg);
      }
      llvm::CallInst *newCall =
          llvm::CallInst::Create(mapIndexFunction, args, "pyul_map_index");
      llvm::ReplaceInstWithInst(callInst, newCall);
    }
  }
}

llvm::Value *
YulIntrinsicEmitter::getPointerToStorageVarByName(std::string name) {
  auto structFieldOrder = visitor.currentContract->getStructFieldOrder();
  auto typeMap = visitor.currentContract->getTypeMap();
  auto fieldIt =
      std::find(structFieldOrder.begin(), structFieldOrder.end(), name);
  assert(fieldIt != structFieldOrder.end());
  int structIndex = fieldIt - structFieldOrder.begin();
  llvm::SmallVector<llvm::Value *> indices;
  indices.push_back(
      llvm::ConstantInt::get(visitor.getContext(), llvm::APInt(32, 0, false)));
  indices.push_back(llvm::ConstantInt::get(
      visitor.getContext(), llvm::APInt(32, structIndex, false)));
  llvm::Value *ptr = visitor.getBuilder().CreateGEP(
      visitor.getSelfType(), (llvm::Value *)visitor.getSelf(), indices,
      "ptr_self_" + name);
  return ptr;
}

llvm::Value *
YulIntrinsicEmitter::emitStorageLoadIntrinsic(YulFunctionCallNode &node) {
  auto &args = node.getArgs();
  assert(args.size() == 2);
  assert(args[0]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  assert(args[1]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  YulLiteralNode &lit0 = (YulLiteralNode &)(*(args[0]));
  YulLiteralNode &lit1 = (YulLiteralNode &)(*(args[1]));
  assert(lit0.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  assert(lit1.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  YulStringLiteralNode &varLit = (YulStringLiteralNode &)(*(args[0]));
  int bitWidth =
      std::get<1>(visitor.currentContract->getTypeMap()[varLit.to_string()]);
  llvm::Value *ptr = getPointerToStorageVarByName(varLit.to_string());
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
  llvm::Value *ptr = getPointerToStorageVarByName(varLit.to_string());
  llvm::Value *storeValue = visitor.visit(valueNode);
  /**
   * llvm::Type *loadType = llvm::Type::getIntNTy(*TheContext, 256);
   * @todo fix all bit widths;
   */
  visitor.getBuilder().CreateStore(storeValue, ptr, false);
}