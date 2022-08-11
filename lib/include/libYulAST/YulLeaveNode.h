#pragma once

#include<libYulAST/YulStatementNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulLeaveNode: protected YulStatementNode{
    public:
        virtual std::string to_string() override;
        virtual void parseRawAST() override;
        YulLeaveNode(nlohmann::json *rawAST);
};
}; // namespace yulast