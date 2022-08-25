#include <cassert>
#include <iostream>
#include <libYulAST/YulNumberLiteralNode.h>
#include <string>
using namespace yulast;

llvm::APInt &YulNumberLiteralNode::getLiteralValue() { return literalValue; }

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
   * @note We are assuming input yul code has only one type i.e.
   * uint256. Therefore, negative numbers in yul are encoded in 2s complement.
   * While using getAsInteger parses it correctly and thereofre `We dont have
   * to worry about sign-bit and types`
   *
   * Further down the line we will have to argue in code about these things and
   * Carefully extend the correct sign bit to the bitwidth to represent the
   * correct number.
   *
   * We will need to propagate types to literals, to form a well-typed ast.
   *
   */
  std::string valString = child["children"][0].get<std::string>();

  if (child["type"] == YUL_DEC_NUMBER_LITERAL_KEY) {
    bool literalParseError =
        llvm::StringRef(valString).getAsInteger(10, literalValue);
    assert(!literalParseError && "Could not parse dec literal");
    if (literalParseError) {
      llvm::WithColor::error() << "Could not parse dec literal";
    }
    literalValue = literalValue.zextOrTrunc(256);
  } else if (child["type"] == YUL_HEX_NUMBER_LITERAL_KEY) {
    bool literalParseError =
        llvm::StringRef(valString).getAsInteger(0, literalValue);
    assert(!literalParseError && "Could not parse hex literal");
    if (literalParseError) {
      llvm::WithColor::error() << "Could not parse hex literal";
    }
    literalValue = literalValue.zextOrTrunc(256);
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
  return llvm::ConstantInt::get(*TheContext, literalValue);
}

std::string YulNumberLiteralNode::to_string() {
  llvm::SmallString<256> litStr;
  literalValue.toStringUnsigned(litStr, 16);
  return "0x" + (std::string)litStr;
}
