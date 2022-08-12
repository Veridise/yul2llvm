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

llvm::Value* YulBlockNode::codegen(){
    llvm::Value *tmp=nullptr;
    for(auto s:statements){
        std::cout<<s->to_string()<<std::endl;
        if(s->statementType == YUL_AST_STATEMENT_EXPRESSION){
            if( ((YulExpressionNode*)s)->expressionType == YUL_AST_EXPRESSION_FUNCTION_CALL){
                YulFunctionCallNode *expr = (YulFunctionCallNode*)s;
                    tmp = expr->codegen();
            }
        }
        else if(s->statementType == YUL_AST_STATEMENT_VARIABLE_DECLARATION){
            ((YulVariableDeclarationNode*)s)->codegen();
        }
        else if(s->statementType == YUL_AST_STATEMENT_ASSIGNMENT){
            ((YulAssignmentNode*)s)->codegen();
        }
    }
    return tmp;
}