#include <cassert>
#include <iostream>
#include <libYulAST/YulVariableDeclarationNode.h>

using namespace yulast;

void YulVariableDeclarationNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() <= 2);
  variableNames =
      std::make_unique<YulTypedIdentifierListNode>(&topLevelChildren[0]);
  if (topLevelChildren.size() == 2) {
    value = YulExpressionBuilder::Builder(&(topLevelChildren[1]));
  }
}

YulVariableDeclarationNode::YulVariableDeclarationNode(const json *rawAST)
    : YulStatementNode(
          rawAST,
          YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_VARIABLE_DECLARATION) {
  assert(sanityCheckPassed(rawAST, YUL_VARIABLE_DECLARATION_KEY));
  parseRawAST(rawAST);
}

std::string YulVariableDeclarationNode::to_string() {
  str.append("var ");
  str.append(variableNames->to_string());
  str.append("=");
  if (value != NULL)
    str.append(value->to_string());
  return str;
}

std::vector<std::unique_ptr<YulIdentifierNode>> &
YulVariableDeclarationNode::getVars() {
  assert(variableNames != NULL);
  return variableNames->getIdentifiers();
}

bool YulVariableDeclarationNode::hasValue() {
  if (value == nullptr)
    return false;
  return true;
}

YulExpressionNode &YulVariableDeclarationNode::getValue() { return *value; }