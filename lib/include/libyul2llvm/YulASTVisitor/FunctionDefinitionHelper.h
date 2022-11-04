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

public:
  void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &node);
  YulFunctionDefinitionHelper(LLVMCodegenVisitor &v, YulIntrinsicHelper &ih);
};