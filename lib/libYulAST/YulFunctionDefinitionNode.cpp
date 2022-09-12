#include <cassert>
#include <fstream>
#include <iostream>
#include <libYulAST/YulFunctionDefinitionNode.h>

using namespace yulast;

void YulFunctionDefinitionNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 2);
  for (json::iterator it = topLevelChildren.begin();
       it != topLevelChildren.end(); it++) {
    if (!(*it)["type"].get<std::string>().compare(YUL_IDENTIFIER_KEY))
      functionName = std::make_unique<YulIdentifierNode>(&(*it));
    else if (!(*it)["type"].get<std::string>().compare(
                 YUL_FUNCTION_ARG_LIST_KEY))
      args = std::make_unique<YulFunctionArgListNode>(&(*it));
    else if (!(*it)["type"].get<std::string>().compare(
                 YUL_FUNCTION_RET_LIST_KEY))
      rets = std::make_unique<YulFunctionRetListNode>(&(*it));
    else if (!(*it)["type"].get<std::string>().compare(YUL_BLOCK_KEY))
      body = std::make_unique<YulBlockNode>(&(*it));
  }
}

YulFunctionDefinitionNode::YulFunctionDefinitionNode(const json *rawAST)
    : YulStatementNode(
          rawAST,
          YUL_AST_STATEMENT_NODE_TYPE::YUL_AST_STATEMENT_FUNCTION_DEFINITION) {
  assert(sanityCheckPassed(rawAST, YUL_FUNCTION_DEFINITION_KEY));
  parseRawAST(rawAST);
}

std::string YulFunctionDefinitionNode::to_string() {
  if (!str.compare("")) {
    str.append("define ");
    str.append(functionName->to_string());
    str.append("(");
    if (args != NULL)
      str.append(args->to_string());
    str.append(")");
    if (rets != NULL)
      str.append(rets->to_string());
    str.append(body->to_string());
    str.append("}");
  }
  return str;
}

void YulFunctionDefinitionNode::createPrototype() {}

std::string YulFunctionDefinitionNode::getName() {
  return functionName->getIdentfierValue();
}

std::vector<std::unique_ptr<YulIdentifierNode>> &
YulFunctionDefinitionNode::getRets() {
  return rets->getIdentifiers();
}
std::vector<std::unique_ptr<YulIdentifierNode>> &
YulFunctionDefinitionNode::getArgs() {
  return args->getIdentifiers();
}

bool YulFunctionDefinitionNode::hasRets() {
  if (rets)
    return true;
  else
    return false;
}

bool YulFunctionDefinitionNode::hasArgs() {
  if (args)
    return true;
  else
    return false;
}

YulBlockNode &YulFunctionDefinitionNode::getBody() { return *body; }