#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulAssignmentNode.h>
#include <libYulAST/YulLeaveNode.h>
#include <libYulAST/YulVariableDeclarationNode.h>
#include <nlohmann/json.hpp>

#include <libYulAST/YulBlockNode.h>
#include <libYulAST/YulExpressionNode.h>
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <libYulAST/YulLiteralNode.h>
#include <libYulAST/YulNumberLiteralNode.h>

namespace yulast {
class YulStatementBuilder {
public:
  static std::unique_ptr<YulStatementNode> Builder(const json *rawAST);
};

class YulExpressionBuilder {
public:
  static std::unique_ptr<YulExpressionNode> Builder(const json *rawAST);
};

class YulLiteralBuilder {
public:
  static std::unique_ptr<YulLiteralNode> Build(const json *rawAST);
};

}; // namespace yulast
