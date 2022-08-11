#pragma once

#include<libYulAST/YulStatementNode.h>
#include<libYulAST/YulTypedIdentifierListNode.h>
#include<libYulAST/YulExpressionNode.h>
#include<libYulAST/YulNodeBuilder.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulVariableDeclarationNode: protected YulStatementNode{
    protected:
        YulTypedIdentifierListNode *variableNames;
        YulExpressionNode *value=NULL;
        virtual void parseRawAST() override;
    public:
        std::string str = "";
        // void codegen() {};
        virtual std::string to_string() override;
        YulVariableDeclarationNode(nlohmann::json *rawAST);
};
};