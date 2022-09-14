#include <cassert>
#include <iostream>
#include <libYulAST/YulLiteralNode.h>
#include <string>
using namespace yulast;

void YulLiteralNode::parseRawAST(const json *rawAST) {
  std::cout << "Literal base; parseRawAST not implemented" << std::endl;
}

YulLiteralNode::YulLiteralNode(const json *rawAST,
                               YUL_AST_LITERAL_NODE_TYPE type)
    : YulExpressionNode(
          rawAST, YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL),
      literalType(type) {}

std::string YulLiteralNode::to_string() { return "Literal base not setup"; }

YUL_AST_LITERAL_NODE_TYPE YulLiteralNode::getLiteralType() {
  return literalType;
}