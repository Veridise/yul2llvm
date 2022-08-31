#pragma once
#include <libYulAST/YulLiteralNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulStringLiteralNode : public YulLiteralNode {
protected:
  std::string literalValue;
  void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual std::string to_string() override;
  virtual llvm::Value *codegen(llvm::Function *F) override;
  YulStringLiteralNode(const json *rawAST);
  /** @todo Currently literals only a data type of 32 bits is assumed
   * Needs to account for other datatypes (more subclasses maybe)
   */
  std::string &getLiteralValue();
};
}; // namespace yulast