#include <cassert>
#include <iostream>
#include <libYulAST/YulVariableDeclarationNode.h>

using namespace yulast;

void YulVariableDeclarationNode::parseRawAST() {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() <= 2);
  variableNames = new YulTypedIdentifierListNode(&topLevelChildren[0]);
  if (topLevelChildren.size() == 2) {
    value = YulExpressionBuilder::Builder(&(topLevelChildren[1]));
  }
}

YulVariableDeclarationNode::YulVariableDeclarationNode(json *rawAST)
    : YulStatementNode(rawAST, YUL_AST_STATEMENT_VARIABLE_DECLARATION) {
  assert(sanityCheckPassed(YUL_VARIABLE_DECLARATION_KEY));
  parseRawAST();
}

std::string YulVariableDeclarationNode::to_string() {
  str.append("var ");
  str.append(variableNames->to_string());
  str.append("=");
  if (value != NULL)
    str.append(value->to_string());
  return str;
}

void YulVariableDeclarationNode::codeGenForOneVar(YulIdentifierNode *id,
                                                  llvm::Function *F) {
  if (NamedValues[id->getIdentfierValue()] != NULL) {
    std::cout << "Error redeclaration of variable " << id->getIdentfierValue()
              << std::endl;
  }
  llvm::AllocaInst *v = CreateEntryBlockAlloca(F, id->getIdentfierValue());
  NamedValues[id->getIdentfierValue()] = v;
}

llvm::Value *YulVariableDeclarationNode::codegen(llvm::Function *F) {
  for (YulIdentifierNode *id : variableNames->getIdentifiers()) {
    codeGenForOneVar(id, F);
    if (value != NULL) {
      llvm::AllocaInst *lval = NamedValues[id->getIdentfierValue()];
      llvm::Value *constant = value->codegen(F);
      Builder->CreateStore(constant, lval);
    }
  }
  return nullptr;
}

std::vector<YulIdentifierNode *> YulVariableDeclarationNode::getVars() {
  assert(variableNames != NULL);
  return variableNames->getIdentifiers();
}