#include <cassert>
#include <iostream>
#include <libYulAST/YulFunctionRetListNode.h>

using namespace yulast;

void YulFunctionRetListNode::parseRawAST(const json *rawAST) {
  json children = rawAST->at("children");
  assert(children.size() == 1);
  json args = children[0];
  identifierList = std::make_unique<YulTypedIdentifierListNode>(&args);
}

YulFunctionRetListNode::YulFunctionRetListNode(const json *rawAST)
    : YulASTBase(rawAST,YUL_AST_NODE_TYPE::YUL_AST_NODE_FUNCTION_ARG_LIST) {
  assert(sanityCheckPassed(rawAST, YUL_FUNCTION_RET_LIST_KEY));
  parseRawAST(rawAST);
}

std::string YulFunctionRetListNode::to_string() {
  if (!str.compare("")) {
    str.append("rets(");
    str.append(identifierList->to_string());
    str.append(")");
  }
  return str;
}

std::vector<std::unique_ptr<YulIdentifierNode>>& YulFunctionRetListNode::getIdentifiers() {
  assert(identifierList != NULL);
  return identifierList->getIdentifiers();
}