#pragma once
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulStatementNode.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace yulast {
class YulBlockNode : public YulStatementNode {
protected:
  std::vector<std::unique_ptr<YulStatementNode>> statements;
  virtual void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual llvm::Value *codegen(llvm::Function *F) override;
  virtual std::string to_string() override;
  YulBlockNode(const json *rawAST);
};
}; // namespace yulast