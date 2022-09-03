#pragma once
#include <libYulAST/YulLiteralNode.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulNumberLiteralNode : public YulLiteralNode {
protected:
  llvm::APInt literalValue;
  void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual std::string to_string() override;
  virtual llvm::Value *codegen(llvm::Function *F) override;
  YulNumberLiteralNode(const json *rawAST);
  /** @todo Currently literals only a data type of 256 bits is assumed
   * Needs to account for other datatypes (more subclasses maybe)
   */
  llvm::APInt &getLiteralValue();
};
}; // namespace yulast