#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulCaseNode.h>
#include <libYulAST/YulDefaultNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulSwitchNode : public YulStatementNode {
protected:
  std::unique_ptr<YulIdentifierNode> condition;
  std::vector<std::unique_ptr<YulCaseNode>> cases;
  std::unique_ptr<YulDefaultNode> defaultNode;
public:
  std::string str = "";
  llvm::Value *codegen(llvm::Function *) override;
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulSwitchNode(const json *rawAST);
  std::unique_ptr<YulIdentifierNode>& getCondition();
  std::vector<std::unique_ptr<YulCaseNode>>& getCases();
  std::unique_ptr<YulDefaultNode>& getDefaultNode();
};
}; // namespace yulast