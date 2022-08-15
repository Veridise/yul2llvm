#include <iostream>
#include <libYulAST/YulExpressionNode.h>

using namespace yulast;

YulExpressionNode::YulExpressionNode(json *rawAST,
                                     YUL_AST_EXPRESSION_TYPE exprType)
    : YulStatementNode(rawAST, YUL_AST_STATEMENT_EXPRESSION),
      expressionType(exprType) {}

llvm::Value *YulExpressionNode::codegen(llvm::Function *F) {
  std::cout
      << "Codegen not implemented for Expression Node, defaulting to base class"
      << std::endl;
  return nullptr;
}

std::string YulExpressionNode::to_string() {
  return "Not implemented for Yul Expression";
}
