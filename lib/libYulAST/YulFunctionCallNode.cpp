#include<libYulAST/YulFunctionCallNode.h>
#include<cassert>
#include<iostream>

using namespace yulast;

void YulFunctionCallNode::parseRawAST() {
    json topLevelChildren = rawAST->at("children");
    assert(topLevelChildren.size()>=1);
    callee = new YulIdentifierNode(&topLevelChildren[0]);
    args = NULL;
    if(topLevelChildren.size()>1){
        args = new YulFunctionArgListNode(&topLevelChildren[1]);
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
        if(args != NULL){
            str.append(args->to_string());
        }
        str.append(")");
    }
    return str;
}

void YulFunctionCallNode::createPrototype(){
     int numargs;
    if(args==NULL ||
        args->identifierList == NULL)
        numargs = 0;
    else    
        numargs = args->identifierList->identifierList.size();

    std::vector<llvm::Type*> funcArgTypes(numargs,
         llvm::Type::getInt32Ty(*TheContext));

    FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext), funcArgTypes, false);

    F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, callee->getIdentfierValue(),
        TheModule.get());

    int idx =0;
    for(auto &arg: F->args()){
        arg.setName(args->identifierList    
            ->identifierList.at(idx++)->getIdentfierValue());
    }
}

llvm::Value * YulFunctionCallNode::codegen(){
    if(!F)
        createPrototype();
    std::vector<llvm::Value *> ArgsV;
    for(auto &a:F->args()){
        ArgsV.push_back(&a);
    }
    std::cout<<"Creating call "<<callee->getIdentfierValue()<<std::endl;
    return Builder->CreateCall(F, ArgsV, callee->getIdentfierValue());
}