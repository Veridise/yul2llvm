#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulIdentifierListNode: protected YulASTBase{
    protected:
        std::vector<YulIdentifierNode*> identifierList;
        void parseRawAST() override;
    public:
        std::string str= "";
        virtual std::string to_string() override;
        YulIdentifierListNode(nlohmann::json *rawAST);
};
};