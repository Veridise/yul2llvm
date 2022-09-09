#include <cassert>
#include <iostream>
#include <libYulAST/YulStatementNode.h>

using namespace yulast;

YulStatementNode::YulStatementNode(const json *rawAST,
                                   YUL_AST_STATEMENT_NODE_TYPE statementType)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_STATEMENT),
      statementType(statementType) {}

llvm::Value *YulStatementNode::codegen(llvm::Function *F) {
  std::cout << "Codegen not defined for YulStatementNode Base" << std::endl;
  return nullptr;
}

std::string YulStatementNode::to_string() {
  return "to_string not implemented for statement";
}

YUL_AST_STATEMENT_NODE_TYPE YulStatementNode::getStatementType(){
  return statementType;
}