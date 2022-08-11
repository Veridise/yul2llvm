#pragma once
#include<libYulAST/YulExpressionNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulIdentifierNode: protected YulExpressionNode{
    protected:
        std::string identifierValue = "";
        void parseRawAST() override;
    public:
        std::string str = "";
        virtual std::string to_string() override;
        YulIdentifierNode(nlohmann::json *rawAST);
        std::string getIdentfierValue();
};
};