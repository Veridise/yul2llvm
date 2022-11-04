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

public:
  void createPrototype();
  std::string str = "";
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulFunctionCallNode(const json *rawAST);
  std::string_view getCalleeName();
  void setCalleeName(std::string_view);
  std::vector<std::unique_ptr<YulExpressionNode>> &getArgs();
};
}; // namespace yulast