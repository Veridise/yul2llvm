#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulTypedIdentifierListNode: protected YulASTBase{
    protected:
        void parseRawAST() override;
    public:
        std::vector<YulIdentifierNode*> identifierList;
        std::string str= "";
        virtual std::string to_string() override;
        YulTypedIdentifierListNode(nlohmann::json *rawAST);
};
};