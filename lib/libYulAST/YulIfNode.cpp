#include <cassert>
#include <iostream>
#include <libYulAST/YulIfNode.h>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

void YulIfNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 2);
  condition = YulExpressionBuilder::Builder(&topLevelChildren[0]);
  thenBody = std::make_unique<YulBlockNode>(&topLevelChildren[1]);
}

YulIfNode::YulIfNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_IF) {
  assert(sanityCheckPassed(rawAST, YUL_IF_KEY));
  parseRawAST(rawAST);
}

std::string YulIfNode::to_string() {
  if (!str.compare("")) {
    str.append("if");
    str.append("(");
    str.append(condition->to_string());
    str.append(")");
    str.append("{");
    str.append(thenBody->to_string());
    str.append("}");
  }
  return str;
}

YulExpressionNode &YulIfNode::getCondition() {
  return *condition;
}

YulBlockNode &YulIfNode::getThenBody() { return *thenBody; }