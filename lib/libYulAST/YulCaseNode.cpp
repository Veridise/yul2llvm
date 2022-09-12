#include <cassert>
#include <iostream>
#include <libYulAST/YulCaseNode.h>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

void YulCaseNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 2);
  json::json_pointer ptr("/0/children/0");
  json literal = topLevelChildren[ptr];
  condition = std::make_unique<YulNumberLiteralNode>(&literal);
  thenBody = std::make_unique<YulBlockNode>(&topLevelChildren[1]);
}

YulCaseNode::YulCaseNode(const json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_CASE) {
  assert(sanityCheckPassed(rawAST, YUL_CASE_KEY));
  parseRawAST(rawAST);
}

std::string YulCaseNode::to_string() {
  if (!str.compare("")) {
    str.append("case ");
    str.append(condition->to_string());
    str.append(":");
    str.append("{\n");
    str.append(thenBody->to_string());
    str.append("}\n");
  }
  return str;
}

llvm::Value *YulCaseNode::codegen(llvm::Function *enclosingFunction) {
  thenBody->codegen(enclosingFunction);
  return nullptr;
}

std::unique_ptr<YulNumberLiteralNode> &YulCaseNode::getCondition() {
  return condition;
}

YulBlockNode &YulCaseNode::getThenBody() { return *thenBody; }