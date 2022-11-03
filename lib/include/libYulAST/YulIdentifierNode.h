#pragma once
#include <libYulAST/YulExpressionNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulIdentifierNode : public YulExpressionNode {
protected:
  std::string identifierValue = "";
  void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual std::string to_string() override;
  YulIdentifierNode(const json *rawAST);
  std::string getIdentfierValue();
  void setIdentifierValue(std::string_view);
};
}; // namespace yulast