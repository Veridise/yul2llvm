#include <cassert>
#include <iostream>
#include <libYulAST/YulStringLiteralNode.h>
#include <llvm/Support/Base64.h>
#include <llvm/Support/SHA1.h>
#include <string>
using namespace yulast;

std::string &YulStringLiteralNode::getLiteralValue() { return literalValue; }

void YulStringLiteralNode::parseRawAST(const json *rawAST) {
  assert(sanityCheckPassed(rawAST, YUL_STRING_LITERAL_KEY));
  literalValue = rawAST->at("children")[0].get<std::string>();
}

YulStringLiteralNode::YulStringLiteralNode(const json *rawAST)
    : YulLiteralNode(rawAST,
                     YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING) {
  parseRawAST(rawAST);
}

llvm::Value *YulStringLiteralNode::codegen(llvm::Function *F) {
  /**
   * @todo @Reviewer: possible perf sink. Any suggestion!
   */
  llvm::SHA1 hasher;
  hasher.update(literalValue);
  std::string literalName =
      "str_lit_" + llvm::encodeBase64(hasher.final()).substr(0, 6);
  if (auto x =
          stringLiteralNames.find(literalValue) != stringLiteralNames.end()) {
    std::string globalName = stringLiteralNames[literalValue];
    return TheModule->getOrInsertGlobal(globalName,
                                        llvm::Type::getInt8Ty(*TheContext));
  }
  return CreateGlobalStringLiteral(literalValue, literalName);
}

std::string YulStringLiteralNode::to_string() { return literalValue; }
