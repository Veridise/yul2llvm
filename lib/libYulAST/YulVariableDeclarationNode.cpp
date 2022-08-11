#include<libYulAST/YulVariableDeclarationNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulVariableDeclarationNode::parseRawAST(){
    json topLevelChildren = rawAST->at("children");
    assert(topLevelChildren.size() <= 2);
    variableNames = new YulTypedIdentifierListNode(&topLevelChildren[0]);
    if(topLevelChildren.size() ==2 ){
        value = YulExpressionBuilder::Builder(&(topLevelChildren[1]));
    }

}

YulVariableDeclarationNode::YulVariableDeclarationNode(
    json *rawAST):YulStatementNode(rawAST, YUL_AST_STATEMENT_VARIABLE_DECLARATION){
        assert(sanityCheckPassed( YUL_VARIABLE_DECLARATION_KEY));
        parseRawAST();
        
}

std::string YulVariableDeclarationNode::to_string(){
    str.append("var ");
    str.append(variableNames->to_string());
    str.append(" ");
    if(value != NULL)
        str.append(value->to_string());
    return str;
}