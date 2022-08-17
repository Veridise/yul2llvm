#pragma once

#include <libYulAST/YulStatementNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulLeaveNode : public YulStatementNode {
public:
  virtual std::string to_string() override;
  virtual void parseRawAST(const json *rawAst) override;
  YulLeaveNode(const json *rawAST);
  virtual llvm::Value *codegen(llvm::Function *F) override { return nullptr; };
};
}; // namespace yulast