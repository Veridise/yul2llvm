#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulFunctionDefinitionNode.h>

class YulFunctionDefinitionHelper {
  LLVMCodegenVisitor &visitor;
  void createVarsForArgsAndRets(YulFunctionDefinitionNode &node,
                                llvm::Function *F);

public:
  void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &node);
  YulFunctionDefinitionHelper(LLVMCodegenVisitor &v);
};