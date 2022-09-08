#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulAssignmentNode.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulBreakNode.h>
#include <libYulAST/YulCaseNode.h>
#include <libYulAST/YulConstants.h>
#include <libYulAST/YulContinueNode.h>
#include <libYulAST/YulContractNode.h>
#include <libYulAST/YulDefaultNode.h>
#include <libYulAST/YulExpressionNode.h>
#include <libYulAST/YulForNode.h>
#include <libYulAST/YulFunctionArgListNode.h>
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <libYulAST/YulFunctionRetListNode.h>
#include <libYulAST/YulIdentifierListNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <libYulAST/YulIfNode.h>
#include <libYulAST/YulLeaveNode.h>
#include <libYulAST/YulLiteralNode.h>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulNumberLiteralNode.h>
#include <libYulAST/YulStatementNode.h>
#include <libYulAST/YulStringLiteralNode.h>
#include <libYulAST/YulSwitchNode.h>
#include <libYulAST/YulTypedIdentifierListNode.h>
#include <libYulAST/YulVariableDeclarationNode.h>

using namespace yulast;
class YulASTVisitorBase{
    llvm::Value* visitYulASTBase(YulASTBase);
    virtual llvm::Value* visitYulAssignmentNode(YulAssignmentNode);
    virtual void visitYulBlockNode(YulBlockNode);
    virtual void visitYulBreakNode(YulBreakNode);
    virtual void visitYulCaseNode(YulCaseNode);
    virtual void visitYulContinueNode(YulContinueNode);
    virtual void visitYulContractNode(YulContractNode);
    virtual void visitYulDefaultNode(YulDefaultNode);
    virtual llvm::Value* visitYulExpressionNode(YulExpressionNode);
    virtual void visitYulForNode(YulForNode);
    virtual void visitYulFunctionArgListNode(YulFunctionArgListNode);
    virtual llvm::Value* visitYulFunctionCallNode(YulFunctionCallNode);
    virtual void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode);
    virtual void visitYulFunctionRetListNode(YulFunctionRetListNode);
    virtual void visitYulIdentifierListNode(YulIdentifierListNode);
    virtual llvm::Value* visitYulIdentifierNode(YulIdentifierNode);
    virtual void visitYulIfNode(YulIfNode);
    virtual void visitYulLeaveNode(YulLeaveNode);
    virtual llvm::Value* visitYulLiteralNode(YulLiteralNode);
    virtual llvm::Value* visitYulNumberLiteralNode(YulNumberLiteralNode);
    virtual llvm::Value* visitYulStatementNode(YulStatementNode);
    virtual llvm::Value* visitYulStringLiteralNode(YulStringLiteralNode);
    virtual void visitYulSwitchNode(YulSwitchNode);
    virtual void visitYulTypedIdentifierListNode(YulTypedIdentifierListNode);
    virtual void visitYulVariableDeclarationNode(YulVariableDeclarationNode);
};