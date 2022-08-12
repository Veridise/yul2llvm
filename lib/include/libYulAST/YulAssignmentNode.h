#pragma once
#include<libYulAST/YulStatementNode.h>
#include<libYulAST/YulIdentifierListNode.h>
#include<libYulAST/YulNodeBuilder.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulAssignmentNode: protected YulStatementNode{
    protected:
        YulIdentifierListNode *lhs;
        YulExpressionNode *rhs;
    public:
        std::string str = "";
        llvm::Value *codegen();
        virtual std::string to_string() override;
        virtual void parseRawAST() override;
        YulAssignmentNode(nlohmann::json *rawAst);
};
}; // namespace yulast