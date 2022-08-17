#pragma once
#include <libYulAST/YulLiteralNode.h>
#include <nlohmann/json.hpp>


namespace yulast {
class YulNumberLiteralNode : public YulLiteralNode {
protected:
  std::int32_t literalValue = 0;
  void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual std::string to_string() override;
  virtual llvm::Value *codegen(llvm::Function *F) override;
  YulNumberLiteralNode(const json *rawAST);
  /** TODO: Currently literals only a data type of 32 bits is assumed
   * Needs to account for other datatypes (more subclasses maybe)
  */
  std::int32_t getLiteralValue();
};
}; // namespace yulast