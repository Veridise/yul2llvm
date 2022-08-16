#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionArgListNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulFunctionCallNode : protected YulExpressionNode {
protected:
  YulIdentifierNode *callee;
  std::vector<YulExpressionNode*> args;

public:
  void createPrototype();
  llvm::Function *F = nullptr;
  llvm::FunctionType *FT = nullptr;
  std::string str = "";
  llvm::Value *codegen(llvm::Function *) override;
  virtual void parseRawAST() override;
  virtual std::string to_string() override;
  YulFunctionCallNode(nlohmann::json *rawAST);
  std::string getName();
  std::vector<YulExpressionNode *> getArgs();
};
}; // namespace yulast