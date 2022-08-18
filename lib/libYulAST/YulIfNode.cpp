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
    : YulStatementNode(
          rawAST,
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

llvm::Value *YulIfNode::codegen(llvm::Function *enclosingFunction) {
  llvm::BasicBlock *thenBlock =
      llvm::BasicBlock::Create(*TheContext, "then-body", enclosingFunction);
  llvm::BasicBlock *contBlock =
      llvm::BasicBlock::Create(*TheContext, "cont-body");
  // calculate condition
  llvm::Value *cond = condition->codegen(enclosingFunction);
  // create actual branch on condition
  Builder->CreateCondBr(cond, thenBlock, contBlock);

  //emit then 
  Builder->SetInsertPoint(thenBlock);
  thenBody->codegen(enclosingFunction);
  Builder->CreateBr(contBlock);
  // thenBlock = Builder->GetInsertBlock();

  // merge node
  enclosingFunction->getBasicBlockList().push_back(contBlock);
  Builder->SetInsertPoint(contBlock);
  return nullptr;
  // llvm::PHINode *phi = Builder->CreatePHI(llvm::Type::getInt32Ty(*TheContext), 2, "if-join");
  // phi->addIncoming(thenBlock);
  // phi->addIncoming(prevBlock);
}

std::unique_ptr<YulExpressionNode>& YulIfNode::getCondition() {
  return condition;
}

std::unique_ptr<YulBlockNode>& YulIfNode::getThenBody() {
  return thenBody;
}