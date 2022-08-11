#pragma once
#include<libYulAST/YulASTBase.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulStatementNode: protected YulASTBase{
    protected:
        YUL_AST_STATEMENT_NODE_TYPE statementType;
    public:
        virtual std::string to_string() override;
        YulStatementNode(nlohmann::json *rawAST, YUL_AST_STATEMENT_NODE_TYPE statementType);
};
};
