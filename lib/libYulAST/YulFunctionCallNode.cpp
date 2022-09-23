#include <cassert>
#include <iostream>
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulStringLiteralNode.h>

using namespace yulast;

void YulFunctionCallNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 1);
  callee = std::make_unique<YulIdentifierNode>(&topLevelChildren[0]);
  for (unsigned long i = 1; i < topLevelChildren.size(); i++) {
    args.push_back(YulExpressionBuilder::Builder(&topLevelChildren[i]));
  }
  if (!callee->getIdentfierValue().compare("revert")) {
    args.clear();
  }
}

YulFunctionCallNode::YulFunctionCallNode(const json *rawAST)
    : YulExpressionNode(
          rawAST,
          YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_FUNCTION_CALL) {
  assert(sanityCheckPassed(rawAST, YUL_FUNCTION_CALL_KEY));
  parseRawAST(rawAST);
}

std::string YulFunctionCallNode::to_string() {
  if (!str.compare("")) {
    str.append(callee->to_string());
    str.append("(");
    for (auto &arg : args) {
      str.append(arg->to_string()).append(",");
    }
    str.append(")");
  }
  return str;
}

void YulFunctionCallNode::setCalleeName(llvm::StringRef name) {
  callee->setIdentifierValue(name);
}

std::string YulFunctionCallNode::getCalleeName() {
  return callee->getIdentfierValue();
}

std::vector<std::unique_ptr<YulExpressionNode>> &
YulFunctionCallNode::getArgs() {
  return args;
}