#pragma once
#include<libYulAST/YulASTVisitor/VisitorBase.h>

using namespace yulast;
class LLVMCodeGenVisitor: YulASTVisitorBase{
    virtual llvm::Value* visitYulAssignmentNode(YulAssignmentNode) override;
    virtual void visitYulBlockNode(YulBlockNode) override;
    virtual void visitYulBreakNode(YulBreakNode) override;
    virtual void visitYulCaseNode(YulCaseNode) override;
    virtual void visitYulContinueNode(YulContinueNode) override;
    virtual void visitYulContractNode(YulContractNode) override;
    virtual void visitYulDefaultNode(YulDefaultNode) override;
    virtual llvm::Value* visitYulExpressionNode(YulExpressionNode) override;
    virtual void visitYulForNode(YulForNode) override;
    virtual void visitYulFunctionArgListNode(YulFunctionArgListNode) override;
    virtual llvm::Value* visitYulFunctionCallNode(YulFunctionCallNode) override;
    virtual void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode) override;
    virtual void visitYulFunctionRetListNode(YulFunctionRetListNode) override;
    virtual void visitYulIdentifierListNode(YulIdentifierListNode) override;
    virtual llvm::Value* visitYulIdentifierNode(YulIdentifierNode) override;
    virtual void visitYulIfNode(YulIfNode) override;
    virtual void visitYulLeaveNode(YulLeaveNode) override;
    virtual llvm::Value* visitYulLiteralNode(YulLiteralNode) override;
    virtual llvm::Value* visitYulNumberLiteralNode(YulNumberLiteralNode) override;
    virtual llvm::Value* visitYulStatementNode(YulStatementNode) override;
    virtual llvm::Value* visitYulStringLiteralNode(YulStringLiteralNode) override;
    virtual void visitYulSwitchNode(YulSwitchNode) override;
    virtual void visitYulTypedIdentifierListNode(YulTypedIdentifierListNode) override;
    virtual void visitYulVariableDeclarationNode(YulVariableDeclarationNode) override;
};