#include <libYulAST/YulASTVisitor/VisitorBase.h>
using namespace yulast;
llvm::Value *YulASTVisitorBase::visitYulASTBase(YulASTBase *node) {
  // Dispater Goes here
  return nullptr;
}
llvm::Value *
YulASTVisitorBase::visitYulAssignmentNode(YulAssignmentNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulAssignmentNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulBlockNode(YulBlockNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulBlockNode codegen not implemented";
}
void YulASTVisitorBase::visitYulBreakNode(YulBreakNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulBreakNode codegen not implemented";
}
void YulASTVisitorBase::visitYulCaseNode(YulCaseNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulCaseNode codegen not implemented";
}
void YulASTVisitorBase::visitYulContinueNode(YulContinueNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulContinueNode codegen not implemented";
}
void YulASTVisitorBase::visitYulContractNode(YulContractNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulContractNode codegen not implemented";
}
void YulASTVisitorBase::visitYulDefaultNode(YulDefaultNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulDefaultNode codegen not implemented";
}
llvm::Value *
YulASTVisitorBase::visitYulExpressionNode(YulExpressionNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulExpressionNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulForNode(YulForNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulForNode codegen not implemented";
}
void YulASTVisitorBase::visitYulFunctionArgListNode(
    YulFunctionArgListNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulFunctionArgListNode codegen not implemented";
}
llvm::Value *
YulASTVisitorBase::visitYulFunctionCallNode(YulFunctionCallNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulFunctionCallNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulFunctionDefinitionNode codegen not implemented";
}
void YulASTVisitorBase::visitYulFunctionRetListNode(
    YulFunctionRetListNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulFunctionRetListNode codegen not implemented";
}
void YulASTVisitorBase::visitYulIdentifierListNode(
    YulIdentifierListNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulIdentifierListNode codegen not implemented";
}
llvm::Value *
YulASTVisitorBase::visitYulIdentifierNode(YulIdentifierNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulIdentifierNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulIfNode(YulIfNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulIfNode codegen not implemented";
}
void YulASTVisitorBase::visitYulLeaveNode(YulLeaveNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulLeaveNode codegen not implemented";
}
llvm::Value *YulASTVisitorBase::visitYulLiteralNode(YulLiteralNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulLiteralNode codegen not implemented";
  return nullptr;
}
llvm::Value *
YulASTVisitorBase::visitYulNumberLiteralNode(YulNumberLiteralNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulNumberLiteralNode codegen not implemented";
  return nullptr;
}
llvm::Value *YulASTVisitorBase::visitYulStatementNode(YulStatementNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulStatementNode codegen not implemented";
  return nullptr;
}
llvm::Value *
YulASTVisitorBase::visitYulStringLiteralNode(YulStringLiteralNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulStringLiteralNode codegen not implemented";
  return nullptr;
}
void YulASTVisitorBase::visitYulSwitchNode(YulSwitchNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulSwitchNode codegen not implemented";
}
void YulASTVisitorBase::visitYulTypedIdentifierListNode(
    YulTypedIdentifierListNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulTypedIdentifierListNode codegen not implemented";
}
void YulASTVisitorBase::visitYulVariableDeclarationNode(
    YulVariableDeclarationNode *node) {
  llvm::WithColor::error()
      << "AstVisitorBase: YulVariableDeclarationNode codegen not implemented";
}
