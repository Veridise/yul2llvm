#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulFunctionCallNode.h>
#include <llvm/IR/Value.h>
using namespace yulast;
class YulIntrinsicEmitter {
  LLVMCodegenVisitor &visitor;

public:
  bool isFunctionCallIntrinsic(std::string calleeName);
  llvm::Value *handleIntrinsicFunctionCall(YulFunctionCallNode &node);
  llvm::Value *handleAddFunctionCall(YulFunctionCallNode &node);
  llvm::Value *emitStorageLoadIntrinsic(YulFunctionCallNode &node);
  void emitStorageStoreIntrinsic(YulFunctionCallNode &node);
  YulIntrinsicEmitter(LLVMCodegenVisitor &v);
};