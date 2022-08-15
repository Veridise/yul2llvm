#include <cassert>
#include <iostream>
#include <libYulAST/YulFunctionRetListNode.h>

using namespace yulast;

void YulFunctionRetListNode::parseRawAST() {
  json children = rawAST->at("children");
  assert(children.size() == 1);
  json args = children[0];
  identifierList = new YulTypedIdentifierListNode(&args);
}

YulFunctionRetListNode::YulFunctionRetListNode(json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_FUNCTION_ARG_LIST) {
  assert(sanityCheckPassed(YUL_FUNCTION_RET_LIST_KEY));
  parseRawAST();
}

std::string YulFunctionRetListNode::to_string() {
  if (!str.compare("")) {
    str.append("rets(");
    str.append(identifierList->to_string());
    str.append(")");
  }
  return str;
}

std::vector<YulIdentifierNode *> YulFunctionRetListNode::getIdentifiers() {
  assert(identifierList != NULL);
  return identifierList->getIdentifiers();
}