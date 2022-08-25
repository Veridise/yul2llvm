#include <cassert>
#include <iostream>
#include <libYulAST/YulNumberLiteralNode.h>
#include <string>
using namespace yulast;

llvm::APInt &YulNumberLiteralNode::getLiteralValue() { return *literalValue; }

void YulNumberLiteralNode::parseRawAST(const json *rawAST) {
  assert(sanityCheckPassed(rawAST, YUL_NUMBER_LITERAL_KEY));
  /**
   * @todo Number literal assumed is only of type yul_dec_number
   * Consider adding a different number type.
   */
  json children = rawAST->at("children");
  assert(children.size() == 1);
  json child = children[0];
  /**
   * @todo If the uint256 literals are encountered, they are
   * set to int max.
   *
   */
  std::string valString = child["children"][0].get<std::string>();

  if (child["type"] == YUL_DEC_NUMBER_LITERAL_KEY) {
    try {
      literalValue = std::make_unique<llvm::APInt>(256, valString, 10);
    } catch (std::exception e) {
      assert(false && "Could not parse decimal number literal");
    }
  } else if (child["type"] == YUL_HEX_NUMBER_LITERAL_KEY) {
    try {
      literalValue = std::make_unique<llvm::APInt>(256, valString, 16);
    } catch (std::exception e) {
      assert(false && "Could not parse hex-number literal");
    }
  } else {
    assert(false && "Unimplemented type of number node");
  }
}

YulNumberLiteralNode::YulNumberLiteralNode(const json *rawAST)
    : YulLiteralNode(rawAST,
                     YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_NUMBER) {
  parseRawAST(rawAST);
}

llvm::Value *YulNumberLiteralNode::codegen(llvm::Function *F) {
  return llvm::ConstantInt::get(*TheContext, *literalValue);
}

std::string YulNumberLiteralNode::to_string() {
  llvm::SmallString<256> litStr;
  literalValue->toStringUnsigned(litStr, 16);
  return (std::string)litStr;
}
