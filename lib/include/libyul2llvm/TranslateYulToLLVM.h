#pragma once
#include <libYulAST/YulContractNode.h>
#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yul2llvm {

class TranslateYulToLLVM {

  json rawContract;
  bool buildContract();
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
