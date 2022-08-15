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
        std::string str = "";
    public:
        virtual llvm::Value *codegen(llvm::Function *F) override;
        void codeGenForOneVar(YulIdentifierNode *id, llvm::Function *F);
        virtual std::string to_string() override;
        YulVariableDeclarationNode(nlohmann::json *rawAST);
        std::vector<YulIdentifierNode*> getVars();
};
};