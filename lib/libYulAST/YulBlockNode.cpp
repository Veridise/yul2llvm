#include <cassert>
#include <iostream>
#include <libYulAST/YulBlockNode.h>

using namespace yulast;

void YulBlockNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  for (json::iterator it = topLevelChildren.begin();
       it != topLevelChildren.end(); it++) {
    std::unique_ptr<YulStatementNode> builtStatement =
        YulStatementBuilder::Builder(&(*it));
    if (builtStatement == NULL) {
      // skipping building some statement take care of it
    } else
      statements.push_back(std::move(builtStatement));
  }
}

YulBlockNode::YulBlockNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_BLOCK) {
  assert(sanityCheckPassed(rawAST, YUL_BLOCK_KEY));
  parseRawAST(rawAST);
}

std::string YulBlockNode::to_string() {
  if (!str.compare("")) {
    str.append("{\n");
    for (auto it = statements.begin(); it != statements.end(); it++) {
      str.append((*it)->to_string()).append("\n");
    }
    str.append("}\n");
  }
  return str;
}

llvm::Value *YulBlockNode::codegen(llvm::Function *F) {
  for (auto &s : statements) {
    s->codegen(F);
  }
  return nullptr;
}

std::vector<std::unique_ptr<YulStatementNode>> &YulBlockNode::getStatements() {
  return statements;
}