#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulASTVisitor/IntrinsicEmitter.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Attributes.h>

class YulFunctionCallHelper {
  LLVMCodegenVisitor &visitor;
  // Intrinsics
  YulIntrinsicEmitter intrinsicEmitter;

public:
  llvm::Type *getReturnType(YulFunctionCallNode &node);
  llvm::SmallVector<llvm::Type *>
  getFunctionArgTypes(llvm::SmallVector<llvm::Value *> &argsV);
  llvm::Function *
  createPrototype(YulFunctionCallNode &node,
                  llvm::SmallVector<llvm::Attribute::AttrKind> &attrs,
                  llvm::SmallVector<llvm::Value *> &argsV);
  std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>>
  buildFunctionAttributes(YulFunctionCallNode &node);
  llvm::Value *visitYulFunctionCallNode(YulFunctionCallNode &node);
  YulFunctionCallHelper(LLVMCodegenVisitor &visitor);
};