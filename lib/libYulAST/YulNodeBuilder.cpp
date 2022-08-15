#include <cassert>
#include <iostream>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

YulStatementNode *YulStatementBuilder::Builder(json *rawAST) {
  assert(rawAST->contains("type"));
  std::string type = rawAST->at("type").get<std::string>();
  if (!type.compare(YUL_ASSIGNMENT_KEY)) {
    return (YulStatementNode *)new YulAssignmentNode(rawAST);
  } else if (!type.compare(YUL_VARIABLE_DECLARATION_KEY)) {
    return (YulStatementNode *)new YulVariableDeclarationNode(rawAST);
  } else if (!type.compare(YUL_LEAVE_KEY)) {
    return (YulStatementNode *)new YulLeaveNode(rawAST);
  } else if (!type.compare(YUL_FUNCTION_CALL_KEY)) {
    return (YulStatementNode *)new YulFunctionCallNode(rawAST);
  } else if (!type.compare(YUL_LEAVE_KEY)) {
    return (YulStatementNode *)new YulLeaveNode(rawAST);
  }
  assert(false);
  return NULL;
}

YulExpressionNode *YulExpressionBuilder::Builder(json *rawAST) {
  assert(rawAST->contains("type"));
  std::string type = rawAST->at("type").get<std::string>();
  if (!type.compare(YUL_FUNCTION_CALL_KEY)) {
    return (YulExpressionNode *)new YulFunctionCallNode(rawAST);
  } else if (!type.compare(YUL_IDENTIFIER_KEY)) {
    return (YulExpressionNode *)new YulIdentifierNode(rawAST);
  }
  assert(false);
  return NULL;
}
