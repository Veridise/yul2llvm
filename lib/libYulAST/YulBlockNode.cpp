#include<libYulAST/YulBlockNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulBlockNode::parseRawAST(){
    json topLevelChildren = rawAST->at("children");
    for(json::iterator it = topLevelChildren.begin(); it != topLevelChildren.end(); it++){
        statements.push_back(YulStatementBuilder::Builder(&(*it)));
    }
}

YulBlockNode::YulBlockNode(json *rawAST):YulStatementNode(rawAST, YUL_AST_STATEMENT_BLOCK){
    assert(sanityCheckPassed(YUL_BLOCK_KEY));
    parseRawAST();
}

std::string YulBlockNode::to_string(){
    if(!str.compare("")){
        str.append("{\n");
        for(std::vector<YulStatementNode*>::iterator it=statements.begin();
            it!=statements.end();it++){
            str.append((*it)->to_string()).append("\n");
        }
        str.append("\n");
    }
    return str;
}