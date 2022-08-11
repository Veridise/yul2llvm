#include<libYulAST/YulStatementNode.h>
#include<cassert>
#include<iostream>


using namespace yulast;

YulStatementNode::YulStatementNode(json *rawAST,
                                   YUL_AST_STATEMENT_NODE_TYPE statementType)
    : YulASTBase(rawAST, YUL_AST_NODE_STATEMENT), statementType(statementType) {

}

std::string YulStatementNode::to_string(){
    return "to_string not implemented for statement";
}

