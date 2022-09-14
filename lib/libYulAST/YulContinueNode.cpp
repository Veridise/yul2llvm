#include <iostream>
#include <libYulAST/YulContinueNode.h>

using namespace yulast;

void YulContinueNode::parseRawAST(const json *rawAST) {}

YulContinueNode::YulContinueNode(const json *rawAST)
    : YulStatementNode(
          rawAST, YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_CONTINUE) {}

std::string YulContinueNode::to_string() { return "break"; }