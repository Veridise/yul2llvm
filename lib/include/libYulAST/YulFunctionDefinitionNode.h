#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulFunctionArgListNode.h>
#include <libYulAST/YulFunctionRetListNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulFunctionDefinitionNode : protected YulStatementNode {
protected:
  YulIdentifierNode *functionName = NULL;
  YulFunctionArgListNode *args = NULL;
  YulFunctionRetListNode *rets = NULL;
  YulBlockNode *body = NULL;
  void createPrototype();
  void createVarsForsRets();
  llvm::FunctionType *FT = NULL;
  llvm::Function *F = NULL;
  std::string str = "";
  virtual void parseRawAST() override;

public:
  void dumpToStdout();
  virtual llvm::Value *codegen(llvm::Function *F) override;
  virtual std::string to_string() override;
  YulFunctionDefinitionNode(nlohmann::json *rawAST);
  void dumpToFile(std::string);
};
}; // namespace yulast