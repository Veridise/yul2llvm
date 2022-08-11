#pragma once

#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionArgListNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulFunctionCallNode: protected YulExpressionNode{
    protected:
        YulIdentifierNode *callee;
        YulFunctionArgListNode *args;
    public:
        void createPrototype();
        llvm::Function *F=nullptr;
        llvm::FunctionType *FT = nullptr;
        std::string str = "";
        llvm::Value* codegen();
        virtual void parseRawAST() override;
        virtual std::string to_string() override;
        YulFunctionCallNode(nlohmann::json *rawAST);
};
}; // namespace yulast