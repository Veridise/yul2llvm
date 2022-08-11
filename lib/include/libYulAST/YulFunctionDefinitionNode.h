#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionArgListNode.h>
#include<libYulAST/YulFunctionRetListNode.h>
#include<libYulAST/YulBlockNode.h>
#include<nlohmann/json.hpp>
#include<llvm/IR/Function.h>
#include<llvm/IR/BasicBlock.h>

namespace yulast{
class YulFunctionDefinitionNode: protected YulStatementNode{
    protected:
        YulIdentifierNode *functionName= NULL;
        YulFunctionArgListNode *args= NULL;
        YulFunctionRetListNode *rets= NULL;
        YulBlockNode *body= NULL;
        void createPrototype();
    public:
        llvm::FunctionType *FT = NULL;
        llvm::Function *F = NULL;
        std::string str = "";
        void codegen();
        virtual void parseRawAST() override;
        virtual std::string to_string() override;
        
        YulFunctionDefinitionNode(nlohmann::json *rawAST);
};
};