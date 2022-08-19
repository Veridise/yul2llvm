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
      conditionNode->getStatements().at(conditionNode->getStatements().size() -
                                        1);
  assert(lastNode->statementType ==
         YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_IF);
  std::unique_ptr<YulIfNode> &ifNode = (std::unique_ptr<YulIfNode> &)lastNode;
  return ifNode->getCondition();
}

llvm::Value *YulForNode::codegen(llvm::Function *enclosingFunction) {

  // initializetion code gen can happen in current basic block
  initializationNode->codegen(enclosingFunction);

  // create termination condtition basic block
  llvm::BasicBlock *condBB = llvm::BasicBlock::Create(
      *TheContext, std::to_string(loopNumber) + "-for-cond", enclosingFunction);
  llvm::BasicBlock *contBB = llvm::BasicBlock::Create(
      *TheContext, std::to_string(loopNumber) + "-for-cont", enclosingFunction);

  Builder->SetInsertPoint(condBB);
  conditionNode->codegen(enclosingFunction);

  // create body basic block
  Builder->GetInsertBlock()->setName(std::to_string(loopNumber) + "-for-body");
  body->codegen(enclosingFunction);

  llvm::BasicBlock *incrBB = llvm::BasicBlock::Create(
      *TheContext, std::to_string(loopNumber) + "-for-incr", enclosingFunction);
  // create increment basic block,
  // creating a separate basic block because cont can jump here
  Builder->SetInsertPoint(incrBB);
  incrementNode->codegen(enclosingFunction);
  Builder->CreateBr(condBB);

  // Reorder the cont basic block
  enclosingFunction->getBasicBlockList().remove(contBB);
  enclosingFunction->getBasicBlockList().push_back(contBB);

  Builder->SetInsertPoint(contBB);
  return nullptr;
}

std::unique_ptr<YulBlockNode> &YulForNode::getConditionNode() {
  return conditionNode;
}

std::unique_ptr<YulBlockNode> &YulForNode::getInitializationNode() {
  return initializationNode;
}
std::unique_ptr<YulBlockNode> &YulForNode::getIncrementNode() {
  return incrementNode;
}
std::unique_ptr<YulBlockNode> &YulForNode::getBody() { return body; }