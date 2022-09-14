#include <libYulAST/YulASTVisitor/VisitorBase.h>
using namespace yulast;
llvm::Value *YulASTVisitorBase::visit(YulASTBase &node) {
  switch (node.getType()) {
  case YUL_AST_NODE_TYPE::YUL_AST_NODE_CONTRACT:
    visitYulContractNode((YulContractNode &)node);
    break;
  case YUL_AST_NODE_TYPE::YUL_AST_NODE_CASE:
    visitYulCaseNode((YulCaseNode &)node);
    break;
  case YUL_AST_NODE_TYPE::YUL_AST_NODE_DEFAULT:
    visitYulDefaultNode((YulDefaultNode &)node);
    break;
  case YUL_AST_NODE_TYPE::YUL_AST_NODE_STATEMENT:
    return visitYulStatementNode((YulStatementNode &)node);
    break;
  default:
    llvm::WithColor::error() << "AstVisitorBase: codegen not implemented";
  }
  return nullptr;
}
llvm::Value *YulASTVisitorBase::visitYulStatementNode(YulStatementNode &node) {
  switch (node.getStatementType()) {
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_ASSIGNMENT:
    visitYulAssignmentNode((YulAssignmentNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_BLOCK:
    visitYulBlockNode((YulBlockNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_FOR:
    visitYulForNode((YulForNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_EXPRESSION:
    return visitYulExpressionNode((YulExpressionNode &)node);

  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_FUNCTION_DEFINITION:
    visitYulFunctionDefinitionNode((YulFunctionDefinitionNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_IF:
    visitYulIfNode((YulIfNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_SWITCH:
    visitYulSwitchNode((YulSwitchNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_VARIABLE_DECLARATION:
    visitYulVariableDeclarationNode((YulVariableDeclarationNode &)node);
    return nullptr;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_BREAK:
    visitYulBreakNode((YulBreakNode &)node);
    break;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_CONTINUE:
    visitYulContinueNode((YulContinueNode &)node);
    break;
  case YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_LEAVE:
    break;
  }
  return nullptr;
}
llvm::Value *
YulASTVisitorBase::visitYulExpressionNode(YulExpressionNode &node) {
  switch (node.getExpressionType()) {
  case YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_FUNCTION_CALL:
    return visitYulFunctionCallNode((YulFunctionCallNode &)node);
    break;
  case YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_IDENTIFIER:
    return visitYulIdentifierNode((YulIdentifierNode &)node);
    break;
  case YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL:
    return visitYulLiteralNode((YulLiteralNode &)node);
    break;
  }
  llvm::WithColor::error()
      << "visitYulExpressionNode: YulExpressionNode codegen not implemented";
  return nullptr;
}
llvm::Value *YulASTVisitorBase::visitYulLiteralNode(YulLiteralNode &node) {
  switch (node.getLiteralType()) {
  case YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_NUMBER:
    return visitYulNumberLiteralNode((YulNumberLiteralNode &)node);
    break;
  case YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING:
    return visitYulStringLiteralNode((YulStringLiteralNode &)node);
  }
  llvm::WithColor::error()
      << "visitYulLiteralNode: YulLiteralNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulAssignmentNode(YulAssignmentNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulAssignmentNode codegen not implemented";
}
void YulASTVisitorBase::visitYulBlockNode(YulBlockNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulBlockNode codegen not implemented";
}
void YulASTVisitorBase::visitYulBreakNode(YulBreakNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulBreakNode codegen not implemented";
}
void YulASTVisitorBase::visitYulCaseNode(YulCaseNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulCaseNode codegen not implemented";
}
void YulASTVisitorBase::visitYulContinueNode(YulContinueNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulContinueNode codegen not implemented";
}
void YulASTVisitorBase::visitYulContractNode(YulContractNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulContractNode codegen not implemented";
}
void YulASTVisitorBase::visitYulDefaultNode(YulDefaultNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulDefaultNode codegen not implemented";
}
void YulASTVisitorBase::visitYulForNode(YulForNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulForNode codegen not implemented";
}
llvm::Value *
YulASTVisitorBase::visitYulFunctionCallNode(YulFunctionCallNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulFunctionCallNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulFunctionDefinitionNode codegen not implemented";
}
llvm::Value *
YulASTVisitorBase::visitYulIdentifierNode(YulIdentifierNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulIdentifierNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulIfNode(YulIfNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulIfNode codegen not implemented";
}
void YulASTVisitorBase::visitYulLeaveNode(YulLeaveNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulLeaveNode codegen not implemented";
}
llvm::Value *
YulASTVisitorBase::visitYulNumberLiteralNode(YulNumberLiteralNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulNumberLiteralNode codegen not implemented";
  return nullptr;
}
llvm::Value *
YulASTVisitorBase::visitYulStringLiteralNode(YulStringLiteralNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulStringLiteralNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulSwitchNode(YulSwitchNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulSwitchNode codegen not implemented";
}
void YulASTVisitorBase::visitYulVariableDeclarationNode(
    YulVariableDeclarationNode &node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulVariableDeclarationNode codegen not implemented";
}
