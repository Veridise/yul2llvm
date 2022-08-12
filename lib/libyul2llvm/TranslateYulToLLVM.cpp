#include "libyul2llvm/TranslateYulToLLVM.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <fstream>
#include <libYulAST/YulConstants.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
using namespace yul2llvm;

TranslateYulToLLVM::TranslateYulToLLVM(std::string filename){
    if(readJsonData(filename)){
        llvm::outs()<<"Could not read json file, exiting \n";
        exit(1);
    }    
}

int TranslateYulToLLVM::readJsonData(std::string filename){
    std::ifstream jsonFile(filename);
    try{
        rawAST = nlohmann::json::parse(jsonFile);   
    } catch(...) {
        llvm::outs()<<"Could not parse json read from ";
        llvm::outs()<<filename;
        return -1;
    }
    return 0;
}

void yul2llvm::TranslateYulToLLVM::run() {
    for (auto it = rawAST.begin(); it != rawAST.end(); ++it)
    {
        llvm::outs()<<(*it).dump()<<"\n\n\n";
        if((*it).contains("type"))
            if(!(*it).at("type").get<std::string>().compare(YUL_FUNCTION_DEFINITION_KEY)){
                yulast::YulFunctionDefinitionNode fundef(&(*it));
                fundef.codegen();
                llvm::outs()<<fundef.to_string();
            }
    }

}
