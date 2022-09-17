#include <cassert>
#include <iostream>
#include <libYulAST/YulForNode.h>
#include <libYulAST/YulIfNode.h>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

void YulForNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() == 4);
  initializationNode = std::make_unique<YulBlockNode>(&topLevelChildren[0]);
  conditionNode = YulExpressionBuilder::Builder(&topLevelChildren[1]);
  incrementNode = std::make_unique<YulBlockNode>(&topLevelChildren[2]);
  body = std::make_unique<YulBlockNode>(&topLevelChildren[3]);
}

YulForNode::YulForNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_FOR) {
  assert(sanityCheckPassed(rawAST, YUL_FOR_KEY));
  parseRawAST(rawAST);
}

std::string YulForNode::to_string() {
  if (!str.compare("")) {
    str.append("for\ninit:\n");
    str.append(initializationNode->to_string());
    str.append("\ncond:\n");
    str.append(conditionNode->to_string());
    str.append("\nbody\n");
    str.append(body->to_string());
    str.append("\nincr:\n");
    str.append(incrementNode->to_string());
    str.append("\nrof:\n");
  }
  return str;
}

/**
 * @todo @warning @Reviewers Check if this casting is okay. static_casting is
 * not working I am a littly sketchy with downcasting unique_pts with
 * static_casting.
 *
 * (std::unique_ptr<YulIfNode> &)lastNode;
 *
 */
YulExpressionNode &YulForNode::getCondition() { return *conditionNode; }

YulBlockNode &YulForNode::getInitializationNode() {
  return *initializationNode;
}
YulBlockNode &YulForNode::getIncrementNode() { return *incrementNode; }
YulBlockNode &YulForNode::getBody() { return *body; }