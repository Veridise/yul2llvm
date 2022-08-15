#pragma once
#include <libYulAST/YulIdentifierListNode.h>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulStatementNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulAssignmentNode : protected YulStatementNode {
protected:
  YulIdentifierListNode *lhs;
  YulExpressionNode *rhs;

public:
  std::string str = "";
  virtual llvm::Value *codegen(llvm::Function *F) override;
  virtual std::string to_string() override;
  virtual void parseRawAST() override;
  YulAssignmentNode(nlohmann::json *rawAst);
  std::vector<YulIdentifierNode *> getLHSIdentifiers();
  YulExpressionNode *getRHSExpression();
};
}; // namespace yulast