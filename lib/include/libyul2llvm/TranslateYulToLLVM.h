#pragma once
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yul2llvm {

class TranslateYulToLLVM {
  /// TODO: this is a placeholder interface, please replace this with the real
  /// interface

  // private fileds
  json rawAST;
  int readJsonData(std::string filename);
  void traverseJson(json);
  std::vector<yulast::YulFunctionDefinitionNode> functions;
  std::string inputFilename, outputFilename;

public:
  TranslateYulToLLVM(std::string inputFilename,
                     std::string outputFilename = NULL);
  void run();
};

} // namespace yul2llvm
