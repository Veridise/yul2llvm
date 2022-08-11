#include<libYulAST/YulASTBase.h>
#include<iostream>

using namespace yulast;

bool YulASTBase::sanityCheckPassed(std::string key){
    if(!rawAST->contains("type")){
        std::cout<<"type not present"<<std::endl;
        return false;
    } 
    if(!rawAST->contains("children")) {
        std::cout<<"children not present"<<std::endl;
        return false;
    }
    if((rawAST->size() != 2)) {
        std::cout<<"size not equal 2"<<std::endl;
        return false;
    }
    if((*rawAST)["type"].get<std::string>().compare(key)){
        std::cout<<"wrong key"<<std::endl;
        return false;
    }
    return true;
}

std::string YulASTBase::to_string(){
    return "to_str not defined for base";
}
    

void YulASTBase::parseRawAST(){
    std::cout<<"Parsing Not Implemented"<<std::endl;
}

YulASTBase::YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodeType):rawAST(rawAST), nodeType(nodeType){
    if(!llvmInitialized){
        // TheContext = std::make_unique<llvm::LLVMContext>();
        // TheModule = std::make_unique<llvm::Module>("my cool jit", *TheContext);
        // Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
        llvmInitialized = true;
    }
}