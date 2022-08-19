#pragma once

#include <libYulAST/YulStatementNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulContinueNode : public YulStatementNode {
public:
  virtual std::string to_string() override;
  virtual void parseRawAST(const json *rawAst) override;
  YulContinueNode(const json *rawAST);
  virtual llvm::Value *codegen(llvm::Function *F) override;
};
}; // namespace yulast