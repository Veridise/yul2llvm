#include <cassert>
#include <iostream>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulSwitchNode.h>

using namespace yulast;

void YulSwitchNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 2);
  condition = YulExpressionBuilder::Builder(&topLevelChildren[0]);

  for (json::size_type i = 1; i < topLevelChildren.size(); i++) {
    json rawCase = topLevelChildren[i];
    if (rawCase["type"] == YUL_CASE_KEY) {
      std::unique_ptr<YulCaseNode> caseNode =
          std::make_unique<YulCaseNode>(&rawCase);
      cases.push_back(std::move(caseNode));
    } else if (rawCase["type"] == YUL_DEFAULT_KEY)
      defaultNode = std::make_unique<YulDefaultNode>(&rawCase);
  }
}

YulSwitchNode::YulSwitchNode(const json *rawAST)
    : YulStatementNode(rawAST,
                       YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_SWITCH) {
  assert(sanityCheckPassed(rawAST, YUL_SWITCH_KEY));
  parseRawAST(rawAST);
}

std::string YulSwitchNode::to_string() {
  if (!str.compare("")) {
    str.append("if");
    str.append("(");
    str.append(condition->to_string());
    str.append(")");
    str.append("{");
    // str.append(thenBody->to_string());
    str.append("}");
  }
  return str;
}

YulExpressionNode &YulSwitchNode::getCondition() { return *condition; }

YulDefaultNode &YulSwitchNode::getDefaultNode() { return *defaultNode; }

std::vector<std::unique_ptr<YulCaseNode>> &YulSwitchNode::getCases() {
  return cases;
}

bool YulSwitchNode::hasDefaultNode() { return defaultNode != nullptr; }