#include "libyul2llvm/TranslateYulToLLVM.h"
#include <fstream>
#include <iostream>

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

void TranslateYulToLLVM::dumpModule(llvm::raw_ostream &stream) const {
  contract->getModule().print(stream, nullptr);
}

void TranslateYulToLLVM::dumpFunctions(llvm::raw_ostream &stream) const {
  for (auto &f : contract->getFunctions()) {
    f->dump(stream);
  }
}

void TranslateYulToLLVM::prettyPrintFunctions(llvm::raw_ostream &stream) {
  for (auto &f : contract->getFunctions()) {
    stream << f->to_string();
  }
}
