#include<libYulAST/YulExpressionNode.h>
#include<libYulAST/YulASTBase.h>

using namespace yulast;

YulExpressionNode::YulExpressionNode(json *rawAST, 
    YUL_AST_EXPRESSION_TYPE exprType) : YulASTBase(rawAST, YUL_AST_NODE_STATEMENT), expressionType(exprType){

    }