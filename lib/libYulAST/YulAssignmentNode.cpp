#include <cassert>
#include <iostream>
#include <libYulAST/YulAssignmentNode.h>

using namespace yulast;

void YulAssignmentNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() == 2);
  // XXX @todo Dont depend on the ordering in children array for type of nodes.
  lhs = std::make_unique<YulIdentifierListNode>(&topLevelChildren[0]);
  rhs = YulExpressionBuilder::Builder(&topLevelChildren[1]);
}

YulAssignmentNode::YulAssignmentNode(const json *rawAST)
    : YulStatementNode(
          rawAST, YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_ASSIGNMENT) {
  assert(sanityCheckPassed(rawAST, YUL_ASSIGNMENT_KEY));
  parseRawAST(rawAST);
}

std::string YulAssignmentNode::to_string() {
  str.append(lhs->to_string());
  str.append("=");
  str.append(rhs->to_string());
  return str;
}

llvm::Value *YulAssignmentNode::codegen(llvm::Function *F) {
  for (auto &var : lhs->getIdentifiers()) {
    std::string lvalname = var->getIdentfierValue();
    llvm::AllocaInst *lval = NamedValues[lvalname];
    if (lval == nullptr) {
      std::cout << "undefined variable " << lvalname;
      exit(1);
    }
    llvm::Value *rval = rhs->codegen(F);
    Builder->CreateStore(rval, lval, false);
  }
  return nullptr;
}

std::vector<std::unique_ptr<YulIdentifierNode>> &
YulAssignmentNode::getLHSIdentifiers() {
  return lhs->getIdentifiers();
}

std::unique_ptr<YulExpressionNode> &YulAssignmentNode::getRHSExpression() {
  return rhs;
}