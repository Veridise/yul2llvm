#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulTypedIdentifierListNode.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yulast {
class YulFunctionArgListNode : public YulASTBase {
protected:
  virtual void parseRawAST(const json *rawAST) override;
  std::unique_ptr<YulTypedIdentifierListNode> identifierList;

public:
  std::string str = "";
  virtual std::string to_string() override;
  YulFunctionArgListNode(const json *rawAST);
  std::vector<std::unique_ptr<YulIdentifierNode>>& getIdentifiers();
};
}; // namespace yulast