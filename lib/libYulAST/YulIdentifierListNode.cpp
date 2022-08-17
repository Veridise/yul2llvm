#include <cassert>
#include <iostream>
#include <libYulAST/YulIdentifierListNode.h>

using namespace yulast;

void YulIdentifierListNode::parseRawAST(const json *rawAST) {
  assert(sanityCheckPassed(rawAST, YUL_IDENTIFIER_LIST_KEY));
  json identifiers = rawAST->at("children");
  for (auto it = identifiers.begin(); it != identifiers.end(); it++) {
    identifierList.push_back(std::make_unique<YulIdentifierNode>(&(*it)));
  }
}

YulIdentifierListNode::YulIdentifierListNode(const json *rawAST)
    : YulASTBase(rawAST,YUL_AST_NODE_TYPE::YUL_AST_NODE_IDENTIFIER_LIST) {
  parseRawAST(rawAST);
}

std::string YulIdentifierListNode::to_string() {
  if (!str.compare("")) {
    for (auto& it: identifierList) {
      str.append(it->to_string());
      str.append(",");
    }
  }
  return str;
}

std::vector<std::unique_ptr<YulIdentifierNode>>& YulIdentifierListNode::getIdentifiers() {
  return identifierList;
}