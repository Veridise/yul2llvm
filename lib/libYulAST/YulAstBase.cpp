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

void YulASTBase::codegen(){
    std::cout<<"Error Codegen not implemented"<<std::endl;
    exit(1);
}

void YulASTBase::parseRawAST(){
    std::cout<<"Parsing Not Implemented"<<std::endl;
}

bool YulASTBase::isParsed(){
    return parsed;
}

void YulASTBase::setParsed(bool val){
    parsed = val;
}

YulASTBase::YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodeType):rawAST(rawAST), nodeType(nodeType){
    
}