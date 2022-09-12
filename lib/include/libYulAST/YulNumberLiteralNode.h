#pragma once
#include <libYulAST/YulLiteralNode.h>
#include <nlohmann/json.hpp>
#include <llvm/Support/WithColor.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/APInt.h>

namespace yulast {
class YulNumberLiteralNode : public YulLiteralNode {
protected:
  llvm::APInt literalValue;
  void parseRawAST(const json *rawAst) override;

public:
  std::string str = "";
  virtual std::string to_string() override;
  YulNumberLiteralNode(const json *rawAST);
  /** @todo Currently literals only a data type of 256 bits is assumed
   * Needs to account for other datatypes (more subclasses maybe)
   */
  llvm::APInt &getLiteralValue();
};
}; // namespace yulast