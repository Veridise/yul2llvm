#pragma once

#include <libYulAST/YulExpressionNode.h>
#include <libYulAST/YulLiteralNode.h>
#include <libYulAST/YulStatementNode.h>
#include <nlohmann/json.hpp>


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
