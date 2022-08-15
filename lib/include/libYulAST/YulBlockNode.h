#pragma once
#include<libYulAST/YulStatementNode.h>
#include<libYulAST/YulNodeBuilder.h>
#include<vector>
#include<nlohmann/json.hpp>

namespace yulast{
class YulBlockNode: protected YulStatementNode{
    protected:
        std::vector<YulStatementNode*> statements;
    public:
        std::string str = "";
        virtual llvm::Value * codegen(llvm::Function *F) override;
        virtual void parseRawAST() override;
        virtual std::string to_string() override;
        YulBlockNode(nlohmann::json *rawAST);
};
}; // namespace yulast