#pragma once
#include <libYulAST/YulExpressionNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulLiteralNode : protected YulExpressionNode {
protected:
  void parseRawAST() override;
  YUL_AST_LITERAL_TYPE literalType;
public:
  std::string str = "";
  virtual std::string to_string() override;
  virtual llvm::Value *codegen(llvm::Function *F) override;
  YulLiteralNode(nlohmann::json *rawAST, YUL_AST_LITERAL_TYPE type);
};
}; // namespace yulast