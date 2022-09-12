#pragma once
#include <libYulAST/YulIdentifierListNode.h>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulStatementNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulAssignmentNode : public YulStatementNode {
protected:
  std::unique_ptr<YulIdentifierListNode> lhs;
  std::unique_ptr<YulExpressionNode> rhs;

public:
  std::string str = "";
  virtual std::string to_string() override;
  virtual void parseRawAST(const json *rawAst) override;
  YulAssignmentNode(const json *rawAst);
  std::vector<std::unique_ptr<YulIdentifierNode>> &getLHSIdentifiers();
  YulExpressionNode &getRHSExpression();
};
}; // namespace yulast