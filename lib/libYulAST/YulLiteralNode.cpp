#include <cassert>
#include <iostream>
#include <libYulAST/YulLiteralNode.h>
#include <string>
using namespace yulast;


void YulLiteralNode::parseRawAST() {
  std::cout<<"Literal base; parseRawAST not implemented"<<std::endl;
}

YulLiteralNode::YulLiteralNode(nlohmann::json *rawAST, YUL_AST_LITERAL_TYPE type)
    : YulExpressionNode(rawAST, YUL_AST_LITERAL), literalType(type) {
}

llvm::Value *YulLiteralNode::codegen(llvm::Function *F) {
  std::cout<<"Literal base: Codegen not implemented"<<std::endl;
  return NULL;
}

std::string YulLiteralNode::to_string() { return "Literal base not setup"; }
