#pragma once
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yul2llvm {

class TranslateYulToLLVM {

  json rawAST;
  void traverseJson(json);
  /**
   * @todo The vecotr function contianing ast definitions of functions is kept
   * here intentionally. We might need access to a metadata/information about
   * a function that is not yet encoded in llvm.
   * This will be eventually removed once we have handled all yul-language
   * features that are currently not supporeted in llvm.
   *
   */
  std::vector<yulast::YulFunctionDefinitionNode> functions;
  std::vector<llvm::Function *> llvmFunctions;
  bool functionsBuilt = false;

public:
  TranslateYulToLLVM(const json rawAST);
  bool areFunctionsBuilt();
  void run();
  void dumpFunctionsToFile(std::string);
};

}; // namespace yul2llvm
