#include <cassert>
#include <iostream>
#include <libYulAST/YulTypedIdentifierListNode.h>

using namespace yulast;

void YulTypedIdentifierListNode::parseRawAST(const json *rawAST) {
  assert(sanityCheckPassed(rawAST, YUL_TYPED_IDENTIFIER_LIST_KEY));
  json identifiers = rawAST->at("children");
  for (json::iterator it = identifiers.begin(); it != identifiers.end(); it++) {
    identifierList.push_back(std::make_unique<YulIdentifierNode>(&(*it)));
  }
}

YulTypedIdentifierListNode::YulTypedIdentifierListNode(const json *rawAST)
    : YulASTBase(rawAST,YUL_AST_NODE_TYPE::YUL_AST_NODE_TYPED_IDENTIFIER_LIST) {
  parseRawAST(rawAST);
}

std::string YulTypedIdentifierListNode::to_string() {
  if (!str.compare("")) {
    for (std::vector<std::unique_ptr<YulIdentifierNode>>::iterator it = identifierList.begin();
         it != identifierList.end(); it++) {
      str.append((*it)->to_string());
      str.append(",");
    }
  }
  return str;
}

std::vector<std::unique_ptr<YulIdentifierNode>>& YulTypedIdentifierListNode::getIdentifiers() {
  return identifierList;
}