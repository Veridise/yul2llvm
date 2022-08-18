#pragma once
#include <libYulAST/YulExpressionNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulLiteralNode : public YulExpressionNode {
protected:
  void parseRawAST(const json *rawAst) override;
  YUL_AST_LITERAL_NODE_TYPE literalType;

public:
  std::string str = "";
  virtual std::string to_string() override;
  virtual llvm::Value *codegen(llvm::Function *F) override;
  YulLiteralNode(const json *rawAST, YUL_AST_LITERAL_NODE_TYPE type);
};
}; // namespace yulast