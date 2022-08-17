#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulTypedIdentifierListNode.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yulast {
class YulFunctionRetListNode : public YulASTBase {
protected:
  virtual void parseRawAST(const json *rawAst) override;
  std::unique_ptr<YulTypedIdentifierListNode> identifierList;
  std::string str = "";

public:
  virtual std::string to_string() override;
  YulFunctionRetListNode(const json *rawAST);
  std::vector<std::unique_ptr<YulIdentifierNode>> &getIdentifiers();
};
}; // namespace yulast