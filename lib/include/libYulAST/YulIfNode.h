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
public:
  std::string str = "";
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulIfNode(const json *rawAST);
  YulExpressionNode &getCondition();
  YulBlockNode &getThenBody();
};
}; // namespace yulast