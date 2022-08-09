#include<libYulAST/YulIdentifierNode.h>

using namespace yulast;

void YulIdentifierNode::parseRawAST() {
    
}

YulIdentifierNode::YulIdentifierNode(json *rawAST):YulExpressionNode(rawAST, YUL_AST_EXPRESSION_IDENTIFIER){    
}