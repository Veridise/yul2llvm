#include<libYulAST/YulFunctionArgListNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulFunctionArgListNode::parseRawAST(){
    json children = rawAST->at("children");
    assert(children.size()==1);
    json args = children[0];
    identifierList = new YulTypedIdentifierListNode(&args);

}

YulFunctionArgListNode::YulFunctionArgListNode(json *rawAST)
    :YulASTBase(rawAST, YUL_AST_NODE_FUNCTION_ARG_LIST){
        assert(sanityCheckPassed(YUL_FUNCTION_ARG_LIST_KEY));
        parseRawAST();
}

std::string YulFunctionArgListNode::to_string(){
    if(!str.compare("")){
        str.append("args(");
        str.append(identifierList->to_string());
        str.append(")");
    }
    return str;
}