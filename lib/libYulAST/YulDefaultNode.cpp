#include <cassert>
#include <iostream>
#include <libYulAST/YulDefaultNode.h>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

void YulDefaultNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 1);
  thenBody = std::make_unique<YulBlockNode>(&topLevelChildren[0]);
}

YulDefaultNode::YulDefaultNode(const json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_DEFAULT) {
  assert(sanityCheckPassed(rawAST, YUL_DEFAULT_KEY));
  parseRawAST(rawAST);
}

std::string YulDefaultNode::to_string() {
  if (!str.compare("")) {
    str.append("default: ");
    str.append("{\n");
    str.append(thenBody->to_string());
    str.append("}\n");
  }
  return str;
}

llvm::Value *YulDefaultNode::codegen(llvm::Function *enclosingFunction) {
  thenBody->codegen(enclosingFunction);
  return nullptr;
}

std::unique_ptr<YulBlockNode> &YulDefaultNode::getThenBody() {
  return thenBody;
}