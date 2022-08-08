#include "libyul2llvm/TranslateYulToLLVM.h"
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <fstream>

using namespace yul2llvm;

TranslateYulToLLVM::TranslateYulToLLVM(std::string filename){
    initializeBuilder();
    if(!readJsonData(filename)){
        llvm::outs()<<"Could not read json file, exiting \n";
        exit(1);
    }    
}

int TranslateYulToLLVM::readJsonData(std::string filename){
    std::ifstream jsonFile(filename);
    nlohmann::json rawAST = nlohmann::json::parse(jsonFile);
    llvm::outs()<<rawAST.dump();
    return 1;
}

void TranslateYulToLLVM::initializeBuilder(){
            Context = std::make_unique<llvm::LLVMContext>();
            Module = std::make_unique<llvm::Module>("Yul2LLVM Translator", *Context);
            Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
} 

void yul2llvm::TranslateYulToLLVM::run(const nlohmann::json &yulAst) {
    llvm::outs()<<"in run()";

}
