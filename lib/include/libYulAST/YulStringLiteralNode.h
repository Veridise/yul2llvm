#pragma once
#include <libYulAST/YulLiteralNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulStringLiteralNode : public YulLiteralNode {
protected:
  std::string literalValue;
  void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual std::string to_string() override;
  YulStringLiteralNode(const json *rawAST);
  std::string &getLiteralValue();
};
}; // namespace yulast