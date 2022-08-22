#include <cassert>
#include <iostream>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulSwitchNode.h>

using namespace yulast;

void YulSwitchNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 2);
  condition = std::make_unique<YulIdentifierNode>(&topLevelChildren[0]);

  json::size_type i = 1;
  for (; i + 1 < topLevelChildren.size(); i++) {
    json rawCase = topLevelChildren[1];
    std::unique_ptr<YulCaseNode> caseNode =
        std::make_unique<YulCaseNode>(&rawCase);
    cases.push_back(std::move(caseNode));
  }

  json rawCase = topLevelChildren[i];
  defaultNode = std::make_unique<YulDefaultNode>(&rawCase);
}

YulSwitchNode::YulSwitchNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_SWITCH) {
  assert(sanityCheckPassed(rawAST, YUL_SWITCH_KEY));
  parseRawAST(rawAST);
}

std::string YulSwitchNode::to_string() {
  if (!str.compare("")) {
    str.append("switch");
    str.append("(");
    str.append(condition->to_string());
    str.append(")");
    for (auto &c : cases) {
      str.append("{");
      str.append(c->to_string());
      str.append("}");
    }
  }
  return str;
}

llvm::Value *YulSwitchNode::codegen(llvm::Function *enclosingFunction) {
  llvm::Value *cond = condition->codegen(enclosingFunction);
  llvm::BasicBlock *defaultBlock =
      llvm::BasicBlock::Create(*TheContext, "default");
  llvm::BasicBlock *cont = llvm::BasicBlock::Create(*TheContext, "switch-cont");

  llvm::SwitchInst *sw =
      Builder->CreateSwitch(cond, defaultBlock, cases.size() + 1);

  for (auto &c : cases) {
    llvm::BasicBlock *caseBB = llvm::BasicBlock::Create(
        *TheContext, c->getCondition()->to_string().append("-case"),
        enclosingFunction);
    int32_t literal = c->getCondition()->getLiteralValue();
    sw->addCase(
        llvm::ConstantInt::get(*TheContext, llvm::APInt(32, literal, 10)),
        caseBB);
    Builder->SetInsertPoint(caseBB);
    c->codegen(enclosingFunction);
    Builder->CreateBr(cont);
  }

  enclosingFunction->getBasicBlockList().push_back(defaultBlock);
  Builder->SetInsertPoint(defaultBlock);
  defaultNode->codegen(enclosingFunction);

  Builder->CreateBr(cont);
  enclosingFunction->getBasicBlockList().push_back(cont);
  Builder->SetInsertPoint(cont);
  return nullptr;
}

std::unique_ptr<YulIdentifierNode> &YulSwitchNode::getCondition() {
  return condition;
}

std::unique_ptr<YulDefaultNode> &YulSwitchNode::getDefaultNode() {
  return defaultNode;
}

std::vector<std::unique_ptr<YulCaseNode>> &YulSwitchNode::getCases() {
  return cases;
}