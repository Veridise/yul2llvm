#include <cassert>
#include <iostream>
#include <libYulAST/YulTypedIdentifierListNode.h>

using namespace yulast;

void YulTypedIdentifierListNode::parseRawAST() {
  assert(sanityCheckPassed(YUL_TYPED_IDENTIFIER_LIST_KEY));
  json identifiers = rawAST->at("children");
  for (json::iterator it = identifiers.begin(); it != identifiers.end(); it++) {
    identifierList.push_back(new YulIdentifierNode(&(*it)));
  }
}

YulTypedIdentifierListNode::YulTypedIdentifierListNode(json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPED_IDENTIFIER_LIST) {
  parseRawAST();
}

std::string YulTypedIdentifierListNode::to_string() {
  if (!str.compare("")) {
    for (std::vector<YulIdentifierNode *>::iterator it = identifierList.begin();
         it != identifierList.end(); it++) {
      str.append((*it)->to_string());
      str.append(",");
    }
  }
  return str;
}

std::vector<YulIdentifierNode *> YulTypedIdentifierListNode::getIdentifiers() {
  return identifierList;
}