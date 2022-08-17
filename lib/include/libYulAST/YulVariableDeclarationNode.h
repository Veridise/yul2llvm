#pragma once

#include <libYulAST/YulExpressionNode.h>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulStatementNode.h>
#include <libYulAST/YulTypedIdentifierListNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulVariableDeclarationNode : public YulStatementNode {
protected:
  std::unique_ptr<YulTypedIdentifierListNode> variableNames;
  std::unique_ptr<YulExpressionNode> value = NULL;
  virtual void parseRawAST(const json *rawAst) override;
  std::string str = "";

public:
  virtual llvm::Value *codegen(llvm::Function *F) override;
  void codeGenForOneVar(std::unique_ptr<YulIdentifierNode> &id,
                        llvm::Function *F);
  virtual std::string to_string() override;
  YulVariableDeclarationNode(const json *rawAST);
  std::vector<std::unique_ptr<YulIdentifierNode>> &getVars();
};
}; // namespace yulast