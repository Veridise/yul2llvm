#include <iostream>
#include <libYulAST/YulBreakNode.h>

using namespace yulast;

void YulBreakNode::parseRawAST(const json *rawAST) {}

YulBreakNode::YulBreakNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_BREAK) {}

llvm::Value *YulBreakNode::codegen(llvm::Function *enclosingFunction) {
  llvm::BasicBlock *breakBB = Builder->GetInsertBlock();
  llvm::BasicBlock *currentBB = breakBB;
  llvm::BasicBlock *contBB = nullptr;
  u_long lineNumberLength;
  std::string loopName;
  while (currentBB && (!currentBB->hasName() ||
                       (lineNumberLength = currentBB->getName().find("for-")) ==
                           std::string::npos)) {
    currentBB = currentBB->getPrevNode();
  }
  assert(currentBB && "break node outside of loop");
  loopName = currentBB->getName().substr(0, lineNumberLength);
  for (auto &bbIt : enclosingFunction->getBasicBlockList())
    if (!bbIt.getName().compare(loopName + "for-cont"))
      contBB = &bbIt;
  assert(contBB && "cannot find for basicblock");
  Builder->CreateBr(contBB);
  return nullptr;
}

std::string YulBreakNode::to_string() { return "break"; }