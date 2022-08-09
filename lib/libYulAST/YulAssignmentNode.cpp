#include<libYulAST/YulAssignmentNode.h>

using namespace yulast;

void YulAssignmentNode::parseRawAST() {
}

YulAssignmentNode::YulAssignmentNode(json *rawAST):YulStatementNode(rawAST, YUL_AST_STATEMENT_ASSIGNMENT){  
}