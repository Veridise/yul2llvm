#include <cassert>
#include <iostream>
#include <libYulAST/YulIdentifierNode.h>
#include <string>
using namespace yulast;

std::string_view YulIdentifierNode::getIdentfierValue() {
  return identifierValue;
}

void YulIdentifierNode::parseRawAST(const json *rawAST) {
  assert(sanityCheckPassed(rawAST, YUL_IDENTIFIER_KEY));
  identifierValue.append(rawAST->at("children")[0].get<std::string>());
}

YulIdentifierNode::YulIdentifierNode(const json *rawAST)
    : YulExpressionNode(
          rawAST, YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_IDENTIFIER) {
  parseRawAST(rawAST);
}

std::string YulIdentifierNode::to_string() { return identifierValue; }

void YulIdentifierNode::setIdentifierValue(std::string_view idValue) {
  identifierValue.assign(idValue);
}