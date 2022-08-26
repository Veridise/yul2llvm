#include "libyul2llvm/TranslateYulToLLVM.h"
#include <fstream>
#include <iostream>
#include <libYulAST/YulConstants.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

using namespace yul2llvm;

TranslateYulToLLVM::TranslateYulToLLVM(const json rawContract, const json rawStorageLayout)
    : rawContract(rawContract), rawStorageLayout(rawStorageLayout) {}

bool TranslateYulToLLVM::sanityCheck(){
  if(!rawContract.contains("type") && rawContract["type"] != "yul_object"){
    llvm::WithColor::error()<<"Ill-formed yul_object";
    // @todo Need better error reporting use llvm::Error s?
    return true;
  }
  if(!rawStorageLayout.is_null()){
    if(!rawContract.contains("storage")){
      llvm::WithColor::error()<<"Ill-formed storageLayout";
      // @todo Need better error reporting use llvm::Error s?
      return true;
    }
  }
}

void TranslateYulToLLVM::buildContract() {
  
}



void TranslateYulToLLVM::run() {
  // std::cout << "[+] Traversing json " << std::endl;
  llvm::ExitOnError(buildContracts());
}

bool TranslateYulToLLVM::areFunctionsBuilt() { return functionsBuilt; }

void TranslateYulToLLVM::dumpFunctions(llvm::raw_ostream &stream) const {
  for (auto &f : functions) {
    f.dump(stream);
  }
}

void TranslateYulToLLVM::prettyPrintFunctions(llvm::raw_ostream &stream) {
  for (auto &f : functions) {
    stream << f.to_string();
  }
}
