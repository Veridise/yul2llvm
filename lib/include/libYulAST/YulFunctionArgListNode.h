#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulTypedIdentifierListNode.h>
#include<nlohmann/json.hpp>
#include<vector>

namespace yulast{
class YulFunctionArgListNode: protected YulASTBase{
    protected:
        virtual void parseRawAST() override;
        YulTypedIdentifierListNode *identifierList;
    public:
        std::string str= "";
        virtual std::string to_string() override;
        YulFunctionArgListNode(nlohmann::json *rawAST);
        std::vector<YulIdentifierNode*> getIdentifiers();

};
};