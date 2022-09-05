#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionArgListNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulFunctionCallNode : public YulExpressionNode {
protected:
  std::unique_ptr<YulIdentifierNode> callee;
  std::vector<std::unique_ptr<YulExpressionNode>> args;
  llvm::Value *emitStorageLoadIntrinsic();
  llvm::Type *getReturnType();
  std::vector<llvm::Type *> getFunctionArgs();
  llvm::Value *emitStorageStoreIntrinsic();

public:
  void createPrototype();
  llvm::Function *F = nullptr;
  llvm::FunctionType *FT = nullptr;
  std::string str = "";
  llvm::Value *codegen(llvm::Function *) override;
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulFunctionCallNode(const json *rawAST);
  std::string getName();
  std::vector<std::unique_ptr<YulExpressionNode>> &getArgs();
};
}; // namespace yulast