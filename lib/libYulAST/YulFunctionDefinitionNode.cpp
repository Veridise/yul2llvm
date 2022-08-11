#include<libYulAST/YulFunctionDefinitionNode.h>
#include<cassert>
#include<iostream>


using namespace yulast;

void YulFunctionDefinitionNode::parseRawAST(){
    json topLevelChildren = rawAST->at("children");
    assert(topLevelChildren.size()>=2);
    for(json::iterator it = topLevelChildren.begin(); 
        it != topLevelChildren.end(); it++){
        if(!(*it)["type"].get<std::string>()
            .compare(YUL_IDENTIFIER_KEY))
            functionName = new YulIdentifierNode(&(*it));
        else if(!(*it)["type"].get<std::string>()
            .compare(YUL_FUNCTION_ARG_LIST_KEY))
            args = new YulFunctionArgListNode(&(*it));
        else if(!(*it)["type"].get<std::string>()
            .compare(YUL_FUNCTION_RET_LIST_KEY))
            rets = new YulFunctionRetListNode(&(*it));
        else if(!(*it)["type"].get<std::string>()
            .compare(YUL_BLOCK_KEY))
            body = new YulBlockNode(&(*it));
    } 
}

YulFunctionDefinitionNode::YulFunctionDefinitionNode(
    json *rawAST):YulStatementNode(rawAST, YUL_AST_STATEMENT_FUNCTION_DEFINITION){
        assert(sanityCheckPassed(YUL_FUNCTION_DEFINITION_KEY));
        parseRawAST();
}

std::string YulFunctionDefinitionNode::to_string(){
    if(!str.compare("")){
        str.append("define ");
        str.append(functionName->to_string());
        str.append("(");
        str.append(args->to_string());
        str.append(")");
        str.append(rets->to_string());
        str.append(body->to_string());
        str.append("}");
    }
    return str;
}