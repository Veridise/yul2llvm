#include <cassert>
#include <iostream>
#include <libYulAST/YulFunctionArgListNode.h>

using namespace yulast;

void YulFunctionArgListNode::parseRawAST(const json *rawAST) {
  json children = rawAST->at("children");
  assert(children.size() == 1);
  json args = children[0];
  identifierList = std::make_unique<YulTypedIdentifierListNode>(&args);
}

YulFunctionArgListNode::YulFunctionArgListNode(const json *rawAST)
    : YulASTBase(rawAST,YUL_AST_NODE_TYPE::YUL_AST_NODE_FUNCTION_ARG_LIST) {
  assert(sanityCheckPassed(rawAST, YUL_FUNCTION_ARG_LIST_KEY));
  parseRawAST(rawAST);
}

std::string YulFunctionArgListNode::to_string() {
  if (!str.compare("")) {
    str.append("args(");
    str.append(identifierList->to_string());
    str.append(")");
  }
  return str;
}

std::vector<std::unique_ptr<YulIdentifierNode>>& YulFunctionArgListNode::getIdentifiers() {
  assert(identifierList != NULL);
  return identifierList->getIdentifiers();
}