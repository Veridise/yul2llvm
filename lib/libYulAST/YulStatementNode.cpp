#include<libYulAST/YulStatementNode.h>
#include<libYulAST/YulASTBase.h>

using namespace yulast;

YulStatementNode::YulStatementNode(json *rawAST, 
    YUL_AST_STATEMENT_NODE_TYPE statementType) : YulASTBase(rawAST, YUL_AST_NODE_STATEMENT), statementType(statementType){

    }