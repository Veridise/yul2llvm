#pragma once
#include<libYulAST/YulASTBase.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulStatementNode: protected YulASTBase{
    protected:
    public:
        YUL_AST_STATEMENT_NODE_TYPE statementType;
        virtual std::string to_string() override;
        virtual llvm::Value * codegen(llvm::Function *F) override;
        YulStatementNode(nlohmann::json *rawAST, YUL_AST_STATEMENT_NODE_TYPE statementType);
};
};
