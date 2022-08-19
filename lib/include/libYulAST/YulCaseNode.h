#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulNumberLiteralNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulCaseNode : public YulASTBase {
protected:
  std::unique_ptr<YulNumberLiteralNode> condition;
  std::unique_ptr<YulBlockNode> thenBody;

public:
  std::string str = "";
  llvm::Value *codegen(llvm::Function *) override;
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulCaseNode(const json *rawAST);
  std::unique_ptr<YulNumberLiteralNode> &getCondition();
  std::unique_ptr<YulBlockNode> &getThenBody();
};
}; // namespace yulast