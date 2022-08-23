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

TranslateYulToLLVM::TranslateYulToLLVM(json inputRawAST)
    : rawAST(inputRawAST) {}

void TranslateYulToLLVM::traverseJson(nlohmann::json j) {
  if (j.is_array()) {
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); it++)
      traverseJson(*it);
  } else if (j.is_object()) {
    if (j.contains("type")) {
      if (!j["type"].get<std::string>().compare(YUL_FUNCTION_DEFINITION_KEY)) {
        yulast::YulFunctionDefinitionNode fundef(&j);
        fundef.codegen(nullptr);
        functions.push_back(std::move(fundef));
        llvmFunctions.push_back(fundef.getLLVMFunction());
        return;
      }
    }
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); it++) {
      traverseJson(it.value());
    }
  }
  functionsBuilt = true;
}

void TranslateYulToLLVM::run() {
  // std::cout << "[+] Traversing json " << std::endl;
  traverseJson(rawAST);
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
