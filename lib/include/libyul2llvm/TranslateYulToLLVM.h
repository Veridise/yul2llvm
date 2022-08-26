#pragma once
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <libYulAST/YulContractNode.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yul2llvm {

class TranslateYulToLLVM {

  json rawContract;
  void buildContract();
  /**
   * @todo The vecotr function contianing ast definitions of functions is kept
   * here intentionally. We might need access to a metadata/information about
   * a function that is not yet encoded in llvm.
   * This will be eventually removed once we have handled all yul-language
   * features that are currently not supporeted in llvm.
   *
   */
  std::unique_ptr<yulast::YulContractNode> contract;
  
  bool sanityCheck();
public:
  TranslateYulToLLVM(const json rawAST);
  void run();
  void dumpFunctions(llvm::raw_ostream &stream = llvm::errs()) const;
  void prettyPrintFunctions(llvm::raw_ostream &stream = llvm::errs());
  void dumpModule(llvm::raw_ostream &stream) const;
};

}; // namespace yul2llvm
