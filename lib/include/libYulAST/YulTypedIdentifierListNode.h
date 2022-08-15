#pragma once
#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulTypedIdentifierListNode: protected YulASTBase{
    protected:
        void parseRawAST() override;
        std::vector<YulIdentifierNode*> identifierList;
    public:
        std::string str= "";
        virtual std::string to_string() override;
        YulTypedIdentifierListNode(nlohmann::json *rawAST);
        std::vector<YulIdentifierNode *> getIdentifiers();
};
};