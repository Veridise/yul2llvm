#include<libYulAST/YulExpressionNode.h>
#include<iostream>

using namespace yulast;

YulExpressionNode::YulExpressionNode(json *rawAST, 
    YUL_AST_EXPRESSION_TYPE exprType) : YulStatementNode(rawAST, YUL_AST_STATEMENT_EXPRESSION), expressionType(exprType){

    }

std::string YulExpressionNode::to_string(){
    return "Not implemented for Yul Expression";
}