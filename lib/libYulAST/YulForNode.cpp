#include <cassert>
#include <iostream>
#include <libYulAST/YulForNode.h>
#include <libYulAST/YulIfNode.h>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

int YulForNode::loopCreated = 0;

void YulForNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() == 4);
  initializationNode = std::make_unique<YulBlockNode>(&topLevelChildren[0]);
  incrementNode = std::make_unique<YulBlockNode>(&topLevelChildren[2]);
  body = std::make_unique<YulBlockNode>(&topLevelChildren[3]);
  conditionNode = std::make_unique<YulBlockNode>(&topLevelChildren[3]);
  /** @todo
   * this is a hacky way to get a block node @reviewer please take a look
   * conditionNode = std::make_unique<YulBlockNode>(&topLevelChildren[3]);
   */
  body->getStatements().clear();
  conditionNode->getStatements().clear();
  std::unique_ptr<YulBlockNode> tmpBody =
      std::make_unique<YulBlockNode>(&topLevelChildren[3]);
  createBodyAndCondition(tmpBody);
}

void YulForNode::createBodyAndCondition(
    std::unique_ptr<YulBlockNode> &tmpBlock) {
  /**
   * @todo
   * Reveiw requested: Here we are assuming that the exit condition of the loop
   * is not implemented in exit condition block of the for loop but
   * is implemented as first if-then and break in body in the body.
   *
   */

  bool loopTerminationBreakHit = false;
  for (auto &stmt : tmpBlock->getStatements()) {
    if (!loopTerminationBreakHit) {
      if (stmt->statementType ==
          YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_IF)
        loopTerminationBreakHit = true;
      conditionNode->getStatements().push_back(std::move(stmt));
    } else {
      body->getStatements().push_back(std::move(stmt));
    }
  }
}

YulForNode::YulForNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_FOR) {
  assert(sanityCheckPassed(rawAST, YUL_FOR_KEY));
  parseRawAST(rawAST);
  loopNumber = loopCreated;
  loopCreated++;
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
std::unique_ptr<YulExpressionNode> &YulForNode::getCondition() {
  std::unique_ptr<YulStatementNode> &lastNode =
      conditionNode->getStatements().back();
  assert(lastNode->statementType ==
         YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_IF);
  std::unique_ptr<YulIfNode> &ifNode = (std::unique_ptr<YulIfNode> &)lastNode;
  return ifNode->getCondition();
}

YulBlockNode &YulForNode::getConditionNode() {
  return *conditionNode;
}

YulBlockNode &YulForNode::getInitializationNode() {
  return *initializationNode;
}
YulBlockNode &YulForNode::getIncrementNode() {
  return *incrementNode;
}
YulBlockNode &YulForNode::getBody() { return *body; }