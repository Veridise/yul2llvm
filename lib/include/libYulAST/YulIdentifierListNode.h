#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulIdentifierListNode : protected YulASTBase {
protected:
  void parseRawAST() override;
  std::vector<YulIdentifierNode *> identifierList;

public:
  std::string str = "";
  std::vector<YulIdentifierNode *> getIdentifiers();
  virtual std::string to_string() override;
  YulIdentifierListNode(nlohmann::json *rawAST);
};
}; // namespace yulast