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

TranslateYulToLLVM::TranslateYulToLLVM(std::string filename,
                                       std::string outputFilename)
    : inputFilename(filename), outputFilename(outputFilename) {
  if (readJsonData(filename)) {
    llvm::outs() << "Could not read json file, exiting \n";
    exit(1);
  }
}

int TranslateYulToLLVM::readJsonData(std::string filename) {
  std::ifstream jsonFileStream(filename);
  try {
    rawAST = nlohmann::json::parse(jsonFileStream);
  } catch (...) {
    llvm::outs() << "Could not parse json read from ";
    llvm::outs() << filename << "\n";
    return -1;
  }
  return 0;
}

void yul2llvm::TranslateYulToLLVM::traverseJson(nlohmann::json j) {
  if (j.is_array()) {
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); it++)
      traverseJson(*it);
  } else if (j.is_object()) {
    if (j.contains("type")) {
      if (!j["type"].get<std::string>().compare(YUL_FUNCTION_DEFINITION_KEY)) {
        yulast::YulFunctionDefinitionNode fundef(&j);
        fundef.codegen(nullptr);
        functions.push_back(std::move(fundef));
        return;
      }
    }
    for (nlohmann::json::iterator it = j.begin(); it != j.end(); it++) {
      traverseJson(it.value());
    }
  }
}

void yul2llvm::TranslateYulToLLVM::run() {
  traverseJson(rawAST);
  for (auto& f : functions) {
    f.dumpToFile(outputFilename);
    // f.dumpToStdout();
  }
}
