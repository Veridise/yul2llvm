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
  } else if (!type.compare(YUL_IF_KEY)) {
    return NULL;
  } else if (!type.compare(YUL_BLOCK_KEY)) {
    return (YulStatementNode *)new YulBlockNode(rawAST);
  }
  std::cout<<"Statement node not implemented: "<<type<<std::endl;
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
  } else if (!type.compare(YUL_LITERAL_KEY)) {
    return (YulExpressionNode *) YulLiteralBuilder::Build(&(rawAST->at("children")[0]));
  }
  assert(false);
  return NULL;
}

YulLiteralNode *YulLiteralBuilder::Build(json *rawAST){
  assert(rawAST->contains("type"));
  std::string type = rawAST->at("type").get<std::string>();
  if (!type.compare(YUL_NUMBER_LITERAL_KEY)) {
    return (YulLiteralNode *) new YulNumberLiteralNode(rawAST);
  }
  assert(false);
  return NULL;
}

