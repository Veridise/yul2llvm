#include "libyul2llvm/TranslateYulToLLVM.h"
#include <fstream>
#include <iostream>

using namespace yul2llvm;

TranslateYulToLLVM::TranslateYulToLLVM(const json rawContract)
    : rawContract(rawContract) {}

bool TranslateYulToLLVM::sanityCheck() {
  if (!rawContract.contains("type") && rawContract["type"] != "yul_object") {
    llvm::WithColor::error() << "Ill-formed yul_object";
    // @todo Need better error reporting use llvm::Error s?
    return false;
  }
  if (!rawContract.contains("metadata")) {
    llvm::WithColor::error() << "Metadata node not present in the contract ast";
    return false;
  } else {
    if (!rawContract["metadata"].contains("state_vars") ||
        !rawContract["metadata"].contains("types")) {
      llvm::WithColor::error() << "Ill-formed storageLayout";
      llvm::WithColor::error() << rawContract["metadata"].dump();
      // @todo Need better error reporting use llvm::Error s?
      return false;
    }
  }
  return true;
}

bool TranslateYulToLLVM::buildContract() {
  bool sanityCheckResult = sanityCheck();
  if (!sanityCheckResult) {
    llvm::WithColor::error() << "Raw json sanity check failed\n";
    return false;
  }
  contract = std::make_unique<yulast::YulContractNode>(&rawContract);
  return true;
}

bool TranslateYulToLLVM::run() {
  // std::cout << "[+] Traversing json " << std::endl;
  if (!buildContract())
    return false;
  return true;
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

llvm::Module *TranslateYulToLLVM::getModule() const {
    return &contract->getModule();
}
