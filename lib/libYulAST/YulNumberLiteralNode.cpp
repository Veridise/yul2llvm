#include <cassert>
#include <iostream>
#include <libYulAST/YulNumberLiteralNode.h>
#include <string>
using namespace yulast;

mpz_class &YulNumberLiteralNode::getLiteralValue() { return literalValue; }

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
    try {
      literalValue.set_str(valString, /*base=*/10);
    } catch (std::invalid_argument &e) {
      assert(false && "Could not parse dec literal");
    }
  } else if (child["type"] == YUL_HEX_NUMBER_LITERAL_KEY) {
    try {
      literalValue.set_str(valString, /*base=*/0);
    } catch (std::invalid_argument &e) {
      assert(false && "Could not parse hex literal");
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

std::string YulNumberLiteralNode::to_string() {
  return "0x" + literalValue.get_str(16);
}
