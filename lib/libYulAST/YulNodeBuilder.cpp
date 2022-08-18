#include <cassert>
#include <iostream>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

std::unique_ptr<YulStatementNode>
YulStatementBuilder::Builder(const json *rawAST) {
  assert(rawAST->contains("type"));
  std::string type = rawAST->at("type").get<std::string>();
  if (!type.compare(YUL_ASSIGNMENT_KEY)) {
    return std::unique_ptr<YulStatementNode>(
        std::make_unique<YulAssignmentNode>(rawAST));
  } else if (!type.compare(YUL_VARIABLE_DECLARATION_KEY)) {
    return std::unique_ptr<YulStatementNode>(
        std::make_unique<YulVariableDeclarationNode>(rawAST));
  } else if (!type.compare(YUL_LEAVE_KEY)) {
    return std::unique_ptr<YulStatementNode>(
        std::make_unique<YulLeaveNode>(rawAST));
  } else if (!type.compare(YUL_FUNCTION_CALL_KEY)) {
    return std::unique_ptr<YulStatementNode>(
        std::make_unique<YulFunctionCallNode>(rawAST));
  } else if (!type.compare(YUL_IF_KEY)) {
    return NULL;
  } else if (!type.compare(YUL_BLOCK_KEY)) {
    return std::unique_ptr<YulStatementNode>(
        std::make_unique<YulBlockNode>(rawAST));
    ;
  }
  std::cout << "Statement node not implemented: " << type << std::endl;
  assert(false && "Statement node not implemented");
  return NULL;
}

std::unique_ptr<YulExpressionNode>
YulExpressionBuilder::Builder(const json *rawAST) {
  assert(rawAST->contains("type"));
  std::string type = rawAST->at("type").get<std::string>();
  if (!type.compare(YUL_FUNCTION_CALL_KEY)) {
    return std::unique_ptr<YulExpressionNode>(
        std::make_unique<YulFunctionCallNode>(rawAST));
  } else if (!type.compare(YUL_IDENTIFIER_KEY)) {
    return std::unique_ptr<YulExpressionNode>(
        std::make_unique<YulIdentifierNode>(rawAST));
  } else if (!type.compare(YUL_LITERAL_KEY)) {
    return YulLiteralBuilder::Build(&(rawAST->at("children")[0]));
  }
  assert(false&& "Encountered an unimplemented Expression");
  return NULL;
}

std::unique_ptr<YulLiteralNode> YulLiteralBuilder::Build(const json *rawAST) {
  assert(rawAST->contains("type"));
  std::string type = rawAST->at("type").get<std::string>();
  if (!type.compare(YUL_NUMBER_LITERAL_KEY)) {
    return std::unique_ptr<YulLiteralNode>(
        std::make_unique<YulNumberLiteralNode>(rawAST));
  }
  assert(false&& "Encountered an unimplemented literal node");
  return NULL;
}