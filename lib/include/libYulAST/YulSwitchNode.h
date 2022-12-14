#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulCaseNode.h>
#include <libYulAST/YulDefaultNode.h>
#include <libYulAST/YulExpressionNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulSwitchNode : public YulStatementNode {
protected:
  std::unique_ptr<YulExpressionNode> condition;
  std::vector<std::unique_ptr<YulCaseNode>> cases;
  std::unique_ptr<YulDefaultNode> defaultNode;

public:
  std::string str = "";
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulSwitchNode(const json *rawAST);
  YulExpressionNode &getCondition();
  YulDefaultNode &getDefaultNode();
  bool hasDefaultNode();
  std::vector<std::unique_ptr<YulCaseNode>> &getCases();
};
}; // namespace yulast