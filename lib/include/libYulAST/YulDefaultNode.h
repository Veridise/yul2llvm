#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulLiteralNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulDefaultNode : public YulASTBase {
protected:
  std::unique_ptr<YulBlockNode> thenBody;

public:
  std::string str = "";
  virtual void parseRawAST(const json *rawAST) override;
  virtual std::string to_string() override;
  YulDefaultNode(const json *rawAST);
  YulBlockNode &getThenBody();
};
}; // namespace yulast