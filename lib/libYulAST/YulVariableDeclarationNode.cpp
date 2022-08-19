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

void YulVariableDeclarationNode::codeGenForOneVar(
    std::unique_ptr<YulIdentifierNode> &id, llvm::Function *F) {
  if (NamedValues[id->getIdentfierValue()] != NULL)
    return;
  llvm::AllocaInst *v = CreateEntryBlockAlloca(F, id->getIdentfierValue());
  NamedValues[id->getIdentfierValue()] = v;
}

llvm::Value *YulVariableDeclarationNode::codegen(llvm::Function *F) {
  for (auto &id : variableNames->getIdentifiers()) {
    codeGenForOneVar(id, F);
    if (value != NULL) {
      llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
      llvm::Value *constant = value->codegen(F);
      Builder->CreateStore(constant, lval);
    }
  }
  return nullptr;
}

std::vector<std::unique_ptr<YulIdentifierNode>> &
YulVariableDeclarationNode::getVars() {
  assert(variableNames != NULL);
  return variableNames->getIdentifiers();
}