#include<libYulAST/YulIdentifierListNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulIdentifierListNode::parseRawAST(){
    assert(sanityCheckPassed(YUL_IDENTIFIER_LIST_KEY));
    json identifiers = rawAST->at("children");
    for(json::iterator it = identifiers.begin(); it != identifiers.end(); it++){
        identifierList.push_back(new YulIdentifierNode(&(*it)));
    }
    
}

void YulIdentifierListNode::codegen(){
    std::cout<<"Codegen not implemented";
}

YulIdentifierListNode::YulIdentifierListNode(
    json *rawAST):YulASTBase(rawAST, YUL_AST_NODE_IDENTIFIER_LIST){
        parseRawAST();
}

std::string YulIdentifierListNode::to_string(){
    if(!str.compare("")){
        for(std::vector<YulIdentifierNode*>::iterator it = identifierList.begin();
            it!=identifierList.end();it++){
            str.append((*it)->to_string());
            str.append(",");
        }
    }
    return str;
}