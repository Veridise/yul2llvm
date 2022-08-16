#include <cassert>
#include <iostream>
#include <libYulAST/YulNumberLiteralNode.h>
#include <string>
using namespace yulast;

std::int32_t YulNumberLiteralNode::getLiteralValue() { return literalValue; }

void YulNumberLiteralNode::parseRawAST() {
  assert(sanityCheckPassed(YUL_NUMBER_LITERAL_KEY));
  /**
   * TODO: Number literal assumed is only of type yul_dec_number
   * Consider adding a different number type.
   */
  json children = rawAST->at("children");
  assert(children.size() == 1);
  json child = children[0];
  /**
   * TODO: If the uint256 literals are encountered, they are 
   * set to int max.
   * 
   */
  std::string valString = child["children"][0].get<std::string>();

  if(child["type"] == YUL_DEC_NUMBER_LITERAL_KEY){
    try{
      literalValue = std::stoi(valString);
    } catch (std::exception e){
      literalValue = INT32_MAX;
    }
  } else if(child["type"] == YUL_HEX_NUMBER_LITERAL_KEY){
    try{
      literalValue = std::stoi(valString, nullptr, 16);
    } catch (std::exception e){
      literalValue = INT32_MAX;
    }
  } else {
    assert(false && "Unimplemented type of number node");
  }
}

YulNumberLiteralNode::YulNumberLiteralNode(nlohmann::json *rawAST)
    : YulLiteralNode(rawAST, YUL_AST_LITERAL_TYPE::YUL_AST_LITERAL_NUMBER) {
  parseRawAST();
}

llvm::Value *YulNumberLiteralNode::codegen(llvm::Function *F) {
  llvm::Type* int32Type = llvm::Type::getInt32Ty(*TheContext);
  return llvm::ConstantInt::get(int32Type, literalValue, true);
}

std::string YulNumberLiteralNode::to_string() { return std::to_string(literalValue); }
