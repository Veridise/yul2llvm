#pragma once
#include <libYulAST/YulASTBase.h>
#include <nlohmann/json.hpp>

namespace yulast {
class YulStatementNode : public YulASTBase {
protected:
public:
  YUL_AST_STATEMENT_NODE_TYPE statementType;
  virtual std::string to_string() override;
  YulStatementNode(const json *rawAST,
                   YUL_AST_STATEMENT_NODE_TYPE statementType);
  YUL_AST_STATEMENT_NODE_TYPE getStatementType();
};
}; // namespace yulast
