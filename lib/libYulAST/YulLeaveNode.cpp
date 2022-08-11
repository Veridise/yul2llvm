#include<libYulAST/YulLeaveNode.h>
#include<iostream>

using namespace yulast;

void YulLeaveNode::parseRawAST(){
    
}

YulLeaveNode::YulLeaveNode(
    json *rawAST):YulStatementNode(rawAST, YUL_AST_STATEMENT_LEAVE){

}

std::string YulLeaveNode::to_string(){
    return "leave";
}