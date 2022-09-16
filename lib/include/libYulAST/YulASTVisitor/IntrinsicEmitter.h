#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulFunctionCallNode.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
using namespace yulast;
class YulIntrinsicEmitter {
  LLVMCodegenVisitor &visitor;

public:
  bool isFunctionCallIntrinsic(std::string calleeName);
  llvm::Value *handleIntrinsicFunctionCall(YulFunctionCallNode &node);
  llvm::Value *handleAddFunctionCall(YulFunctionCallNode &node);
  llvm::Value *emitStorageLoadIntrinsic(YulFunctionCallNode &node);
  llvm::Value *handleMapIndex(YulFunctionCallNode&);
  void emitStorageStoreIntrinsic(YulFunctionCallNode &node);
  llvm::Value *getPointerToStorageVarByName(std::string);
  llvm::FunctionType *getMapIndexFT();
  llvm::Function *getOrCreateFunction(std::string, llvm::FunctionType *);
  YulIntrinsicEmitter(LLVMCodegenVisitor &v);

};