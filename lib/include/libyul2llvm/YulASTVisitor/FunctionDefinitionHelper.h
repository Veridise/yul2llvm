#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>
#include <llvm/IR/Verifier.h>

class YulFunctionDefinitionHelper {
  LLVMCodegenVisitor &visitor;
  void createVarsForArgsAndRets(YulFunctionDefinitionNode &node,
                                llvm::Function *F);
  YulIntrinsicHelper &intrinsicEmitter;
  void addReturnNode(YulFunctionDefinitionNode &node);
  void allocateSelf();

public:
  llvm::Function *createStorageAllocatorFunction();
  llvm::Function *createMemoryAllocatorFunction();
  void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &node);
  YulFunctionDefinitionHelper(LLVMCodegenVisitor &v, YulIntrinsicHelper &ih);
};