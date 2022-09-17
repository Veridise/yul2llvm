#pragma once

#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulDefaultNode.h>
#include <nlohmann/json.hpp>

namespace yulast {

class YulForNode : public YulStatementNode {
protected:
  std::unique_ptr<YulBlockNode> initializationNode;
  std::unique_ptr<YulBlockNode> incrementNode;
  std::unique_ptr<YulBlockNode> body;
  std::unique_ptr<YulExpressionNode> conditionNode;

public:
  std::string str = "";
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulForNode(const json *rawAST);
  YulExpressionNode &getCondition();
  YulBlockNode &getInitializationNode();
  YulBlockNode &getIncrementNode();
  YulBlockNode &getBody();
};
}; // namespace yulast