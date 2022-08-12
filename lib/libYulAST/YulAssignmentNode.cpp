#include<libYulAST/YulAssignmentNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulAssignmentNode::parseRawAST() {
    json topLevelChildren = rawAST->at("children");
    assert(topLevelChildren.size() == 2);
    // XXX TODO: Dont depend on the ordering in children array for type of nodes. 
    lhs = new YulIdentifierListNode(&topLevelChildren[0]);
    rhs =  YulExpressionBuilder::Builder(&topLevelChildren[1]);

}

YulAssignmentNode::YulAssignmentNode(json *rawAST):YulStatementNode(rawAST, YUL_AST_STATEMENT_ASSIGNMENT){  
    assert(sanityCheckPassed(YUL_ASSIGNMENT_KEY));
    parseRawAST();
}

std::string YulAssignmentNode::to_string(){
    str.append(lhs->to_string());
    str.append("=");
    str.append(rhs->to_string());
    return str;

}

llvm::Value * YulAssignmentNode::codegen(){
    
}