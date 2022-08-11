#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulTypedIdentifierListNode.h>
#include<vector>
#include<nlohmann/json.hpp>

namespace yulast{
class YulFunctionRetListNode: protected YulASTBase{
    protected:
        YulTypedIdentifierListNode *identifierList;
        virtual void parseRawAST() override;
    public:
        std::string str = "";
        virtual std::string to_string() override;
        YulFunctionRetListNode(nlohmann::json *rawAST);
};
}; // namespace yulast