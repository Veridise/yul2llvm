#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulAssignmentNode.h>
#include <libYulAST/YulLeaveNode.h>
#include <libYulAST/YulVariableDeclarationNode.h>
#include <nlohmann/json.hpp>

#include <libYulAST/YulExpressionNode.h>
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulIdentifierNode.h>
#include <libYulAST/YulLiteralNode.h>
#include <libYulAST/YulNumberLiteralNode.h>
#include <libYulAST/YulBlockNode.h>

namespace yulast {
class YulStatementBuilder {
public:
  static YulStatementNode *Builder(json *rawAST);
};

class YulExpressionBuilder {
public:
  static YulExpressionNode *Builder(json *rawAST);
};

class YulLiteralBuilder {
public:
  static YulLiteralNode *Build(json *rawAST);
};

}; // namespace yulast
