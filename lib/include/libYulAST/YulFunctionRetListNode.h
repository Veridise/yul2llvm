#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulTypedIdentifierListNode.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yulast {
class YulFunctionRetListNode : protected YulASTBase {
protected:
  virtual void parseRawAST() override;
  YulTypedIdentifierListNode *identifierList;
  std::string str = "";

public:
  virtual std::string to_string() override;
  YulFunctionRetListNode(nlohmann::json *rawAST);
  std::vector<YulIdentifierNode *> getIdentifiers();
};
}; // namespace yulast