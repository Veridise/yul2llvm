#include <cassert>
#include <iostream>
#include <libYulAST/YulIdentifierNode.h>
#include <string>
using namespace yulast;

std::string YulIdentifierNode::getIdentfierValue() { return identifierValue; }

void YulIdentifierNode::parseRawAST() {
  assert(sanityCheckPassed(YUL_IDENTIFIER_KEY));
  identifierValue.append(rawAST->at("children")[0].get<std::string>());
}

YulIdentifierNode::YulIdentifierNode(nlohmann::json *rawAST)
    : YulExpressionNode(rawAST, YUL_AST_EXPRESSION_IDENTIFIER) {
  parseRawAST();
}

llvm::Value *YulIdentifierNode::codegen(llvm::Function *F) {
  llvm::Type *inttype = llvm::Type::getInt32Ty(*TheContext);
  return Builder->CreateLoad(inttype, NamedValues[getIdentfierValue()],
                             getIdentfierValue());
}

std::string YulIdentifierNode::to_string() { return identifierValue; }
