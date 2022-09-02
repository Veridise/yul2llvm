#pragma once
#include <libYulAST/YulConstants.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace yulast {
class YulASTBase {
protected:
  YUL_AST_NODE_TYPE nodeType;
  virtual void parseRawAST(const json *rawAst);

public:
  static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                                  const std::string &VarName);
  static llvm::GlobalVariable *
  CreateGlobalStringLiteral(std::string literalValue, std::string literalName);
  static std::unique_ptr<llvm::LLVMContext> TheContext;
  static std::unique_ptr<llvm::Module> TheModule;
  static std::unique_ptr<llvm::IRBuilder<>> Builder;
  static llvm::StringMap<llvm::AllocaInst *> NamedValues;
  static llvm::StringMap<std::string> stringLiteralNames;
  // data structures for self
  static llvm::SmallVector<std::string> structFieldOrder;
  static llvm::StringMap<std::tuple<std::string, int>> typeMap;
  static llvm::StructType *selfType;
  static llvm::GlobalVariable *self;
  bool llvmInitialized = false;
  virtual ~YulASTBase(){};
  virtual llvm::Value *codegen(llvm::Function *F);
  virtual std::string to_string();
  YulASTBase(const json *rawAST, YUL_AST_NODE_TYPE nodeType);
  bool sanityCheckPassed(const json *rawAST, std::string);
  llvm::Module &getModule();
  llvm::IRBuilder<> &getBuilder();
  llvm::LLVMContext &getContext();
  llvm::StringMap<llvm::AllocaInst *> &getNamedValuesMap();
};
}; // namespace yulast