#include <cassert>
#include <iostream>
#include <libYulAST/YulStringLiteralNode.h>
#include <string>
using namespace yulast;

std::string &YulStringLiteralNode::getLiteralValue() { return literalValue; }

void YulStringLiteralNode::parseRawAST(const json *rawAST) {
  assert(sanityCheckPassed(rawAST, YUL_STRING_LITERAL_KEY));
  literalValue = rawAST->at("children")[0].get<std::string>();
}

YulStringLiteralNode::YulStringLiteralNode(const json *rawAST)
    : YulLiteralNode(rawAST,
                     YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING) {
  parseRawAST(rawAST);
}

std::string YulStringLiteralNode::to_string() { return literalValue; }
