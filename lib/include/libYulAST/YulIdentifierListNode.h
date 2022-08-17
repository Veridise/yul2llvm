#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulIdentifierListNode : public YulASTBase {
protected:
  void parseRawAST(const json *rawAst) override;
  std::vector<std::unique_ptr<YulIdentifierNode>> identifierList;

public:
  std::string str = "";
  std::vector<std::unique_ptr<YulIdentifierNode>> &getIdentifiers();
  virtual std::string to_string() override;
  YulIdentifierListNode(const json *rawAST);
};
}; // namespace yulast