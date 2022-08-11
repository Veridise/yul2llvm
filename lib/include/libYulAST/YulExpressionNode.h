#pragma once
#include<libYulAST/YulStatementNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulExpressionNode: protected YulStatementNode{
    protected:
    public:
        YUL_AST_EXPRESSION_TYPE expressionType;
        virtual std::string to_string() override;
        YulExpressionNode(nlohmann::json *rawAST, YUL_AST_EXPRESSION_TYPE exprType);
        virtual ~YulExpressionNode(){};
};
}; // namespace yulast