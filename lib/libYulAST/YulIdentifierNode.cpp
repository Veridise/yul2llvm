#include<libYulAST/YulIdentifierNode.h>
#include<string>
#include<cassert>
#include<iostream>
using namespace yulast;

std::string YulIdentifierNode::getIdentfierValue(){
    return identifierValue;
}

void YulIdentifierNode::parseRawAST() {
    assert(sanityCheckPassed(YUL_IDENTIFIER_KEY));
    identifierValue.append(rawAST->at("children")[0].get<std::string>());
}

YulIdentifierNode::YulIdentifierNode(nlohmann::json *rawAST):YulExpressionNode(rawAST, YUL_AST_EXPRESSION_IDENTIFIER){
    parseRawAST();
}

void YulIdentifierNode::codegen(){
    std::cout<<"Codegen not implemented";
}

std::string YulIdentifierNode::to_string(){
    return identifierValue;
}