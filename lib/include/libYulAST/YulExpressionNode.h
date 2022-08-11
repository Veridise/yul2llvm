#pragma once
#include<libYulAST/YulStatementNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulExpressionNode: protected YulStatementNode{
    protected:
        YUL_AST_EXPRESSION_TYPE expressionType;
    public:
        virtual std::string to_string() override;
        YulExpressionNode(nlohmann::json *rawAST, YUL_AST_EXPRESSION_TYPE exprType);
        virtual ~YulExpressionNode(){};
};
};