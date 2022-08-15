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
    if(args == NULL)
        numargs = 0;
    else    
        numargs = getArgs().size();

    std::vector<llvm::Type*> funcArgTypes(numargs,
         llvm::Type::getInt32Ty(*TheContext));

    FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext), funcArgTypes, false);

    F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, callee->getIdentfierValue(),
        TheModule.get());
}

llvm::Value* YulFunctionCallNode::codegen(llvm::Function *enclosingFunction){
    if(!F)
        F = TheModule->getFunction(callee->getIdentfierValue());

    if(!F)
        createPrototype();
    else{
        std::cout<<"Function not found and could not be created"<<std::endl;
        exit(1);
    }
    if(!callee->getIdentfierValue().compare("checked_add_t_uint256")){
        llvm::Value *v1, *v2;
        v1 = Builder->CreateLoad(llvm::Type::getInt32Ty(*TheContext),
            NamedValues[args->getIdentifiers()[0]->getIdentfierValue()]);
        v2 = Builder->CreateLoad(llvm::Type::getInt32Ty(*TheContext),
            NamedValues[args->getIdentifiers()[1]->getIdentfierValue()]);
        return  Builder->CreateAdd(v1, v2);
    }
    std::vector<llvm::Value *> ArgsV;

    if(args != nullptr)
        for(auto a:args->getIdentifiers()){
            std::cout<<"Loading identifier "<<a<<std::endl;
            llvm::Value *lv = Builder->CreateLoad(llvm::Type::getInt32Ty(*TheContext), NamedValues[a->getIdentfierValue()]);
            ArgsV.push_back(lv);
        }
    // std::cout<<"Creating call "<<callee->getIdentfierValue()<<std::endl;
    return Builder->CreateCall(F, ArgsV, callee->getIdentfierValue());
}

std::string YulFunctionCallNode::getName(){
    return callee->getIdentfierValue();
}

std::vector<YulIdentifierNode*> YulFunctionCallNode::getArgs(){
    return args->getIdentifiers();
}