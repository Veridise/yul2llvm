#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulTypedIdentifierListNode : public YulASTBase {
protected:
  void parseRawAST(const json *rawAst) override;
  std::vector<std::unique_ptr<YulIdentifierNode>> identifierList;

public:
  std::string str = "";
  virtual std::string to_string() override;
  YulTypedIdentifierListNode(const json *rawAST);
  std::vector<std::unique_ptr<YulIdentifierNode>>& getIdentifiers();
};
}; // namespace yulast