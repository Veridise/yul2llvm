#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulExpressionNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulIfNode : public YulStatementNode {
protected:
  std::unique_ptr<YulExpressionNode> condition;
  std::unique_ptr<YulBlockNode> thenBody;
  static int ifsCreated;
  int ifId;

public:
  void createPrototype();
  std::string str = "";
  llvm::Value *codegen(llvm::Function *) override;
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulIfNode(const json *rawAST);
  std::unique_ptr<YulExpressionNode> &getCondition();
  std::unique_ptr<YulBlockNode> &getThenBody();
};
}; // namespace yulast