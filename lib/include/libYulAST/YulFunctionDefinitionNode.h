#pragma once
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulFunctionArgListNode.h>
#include <libYulAST/YulFunctionRetListNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulFunctionDefinitionNode : public YulStatementNode {
protected:
  std::unique_ptr<YulIdentifierNode> functionName;
  std::unique_ptr<YulFunctionArgListNode> args;
  std::unique_ptr<YulFunctionRetListNode> rets;
  std::unique_ptr<YulBlockNode> body;
  void createPrototype();
  void createVarsForArgsAndRets();
  std::string str = "";
  virtual void parseRawAST(const json *rawAst) override;

public:
  virtual std::string to_string() override;
  YulFunctionDefinitionNode(const json *rawAST);
  std::string_view getName();
  std::vector<std::unique_ptr<YulIdentifierNode>> &getRets();
  std::vector<std::unique_ptr<YulIdentifierNode>> &getArgs();
  YulBlockNode &getBody() const;
  bool hasRets() const;
  bool hasArgs() const;
  int getNumArgs();
};
}; // namespace yulast
