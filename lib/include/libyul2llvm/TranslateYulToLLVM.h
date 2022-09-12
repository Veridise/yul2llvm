#pragma once
#include <libYulAST/YulContractNode.h>
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yul2llvm {

class TranslateYulToLLVM {

  json rawContract;
  bool buildContract();
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
  LLVMCodegenVisitor visitor;

public:
  TranslateYulToLLVM(const json rawAST);
  bool run();
  void prettyPrintFunctions(llvm::raw_ostream &stream = llvm::errs());
  void dumpModule(llvm::raw_ostream &stream);
  llvm::Module &getModule();
};

}; // namespace yul2llvm
