#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/IR/Verifier.h>

class YulFunctionDefinitionHelper {
  LLVMCodegenVisitor &visitor;
  void createVarsForArgsAndRets(YulFunctionDefinitionNode &node,
                                llvm::Function *F);
  YulIntrinsicHelper &intrinsicEmitter;

public:
  void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &node);
  YulFunctionDefinitionHelper(LLVMCodegenVisitor &v, YulIntrinsicHelper &ih);
};