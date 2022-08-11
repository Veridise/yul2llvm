#include<libYulAST/YulFunctionCallNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulFunctionCallNode::parseRawAST() {
    json topLevelChildren = rawAST->at("children");
    assert(topLevelChildren.size()>=1);
    callee = new YulIdentifierNode(&topLevelChildren[0]);
    argList = NULL;
    if(topLevelChildren.size()>1){
        argList = new YulFunctionArgListNode(&topLevelChildren[1]);
    }

}

YulFunctionCallNode::YulFunctionCallNode(json *rawAST):YulExpressionNode(rawAST, YUL_AST_EXPRESSION_FUNCTION_CALL){    
    assert(sanityCheckPassed(YUL_FUNCTION_CALL_KEY));
    parseRawAST();
}

std::string YulFunctionCallNode::to_string(){
    if(!str.compare("")){
        str.append(callee->to_string());
        str.append("(");
        if(argList != NULL){
            str.append(argList->to_string());
        }
        str.append(")");
    }
    return str;
}