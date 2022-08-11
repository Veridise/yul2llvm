#pragma once

#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionArgListNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulFunctionCallNode: protected YulExpressionNode{
    protected:
        YulIdentifierNode *callee;
        YulFunctionArgListNode *argList;
    public:
        std::string str = "";
        virtual void parseRawAST() override;
        virtual std::string to_string() override;
        YulFunctionCallNode(nlohmann::json *rawAST);
};
}; // namespace yulast