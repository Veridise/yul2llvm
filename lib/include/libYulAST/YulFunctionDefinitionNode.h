#pragma once

#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionArgListNode.h>
#include<libYulAST/YulFunctionRetListNode.h>
#include<libYulAST/YulBlockNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulFunctionDefinitionNode: protected YulStatementNode{
    protected:
        YulIdentifierNode *functionName= NULL;
        YulFunctionArgListNode *args= NULL;
        YulFunctionRetListNode *rets= NULL;
        YulBlockNode *body= NULL;
    public:
        std::string str = "";
        virtual void codegen() override {};
        virtual void parseRawAST() override;
        virtual std::string to_string() override;
        
        YulFunctionDefinitionNode(nlohmann::json *rawAST);
};
}; // namespace yulast