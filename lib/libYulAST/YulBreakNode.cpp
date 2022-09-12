#include <iostream>
#include <libYulAST/YulBreakNode.h>

using namespace yulast;

void YulBreakNode::parseRawAST(const json *rawAST) {}

YulBreakNode::YulBreakNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_BREAK) {}

std::string YulBreakNode::to_string() { return "break"; }