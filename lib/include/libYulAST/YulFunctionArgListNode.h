#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulTypedIdentifierListNode.h>
#include<nlohmann/json.hpp>
#include<vector>

namespace yulast{
class YulFunctionArgListNode: protected YulASTBase{
    protected:
        YulTypedIdentifierListNode *identifierList;
        virtual void parseRawAST() override;
        
    public:
        std::string str= "";
        virtual void codegen() override {};
        virtual std::string to_string() override;
        YulFunctionArgListNode(nlohmann::json *rawAST);
};
}; // namespace yulast