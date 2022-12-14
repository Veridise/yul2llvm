#include <cassert>
#include <iostream>
#include <libYulAST/YulStatementNode.h>

using namespace yulast;

YulStatementNode::YulStatementNode(const json *rawAST,
                                   YUL_AST_STATEMENT_NODE_TYPE statementType)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_STATEMENT),
      statementType(statementType) {}

std::string YulStatementNode::to_string() {
  return "to_string not implemented for statement";
}

YUL_AST_STATEMENT_NODE_TYPE YulStatementNode::getStatementType() const {
  return statementType;
}