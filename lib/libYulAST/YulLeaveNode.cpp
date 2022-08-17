#include <iostream>
#include <libYulAST/YulLeaveNode.h>

using namespace yulast;

void YulLeaveNode::parseRawAST(const json *rawAST) {}

YulLeaveNode::YulLeaveNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_LEAVE) {}

std::string YulLeaveNode::to_string() { return "leave"; }