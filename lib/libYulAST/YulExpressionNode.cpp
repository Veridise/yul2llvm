#include <iostream>
#include <libYulAST/YulExpressionNode.h>

using namespace yulast;

YulExpressionNode::YulExpressionNode(const json *rawAST,
                                     YUL_AST_EXPRESSION_NODE_TYPE exprType)
    : YulStatementNode(
          rawAST, YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_EXPRESSION),
      expressionType(exprType) {}

std::string YulExpressionNode::to_string() {
  return "Not implemented for Yul Expression";
}

YUL_AST_EXPRESSION_NODE_TYPE YulExpressionNode::getExpressionType(){
  return expressionType;
}