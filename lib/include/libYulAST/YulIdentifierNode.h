#include<libYulAST/YulExpressionNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulIdentifierNode: protected YulExpressionNode{
    protected:
        const std::string identifierValue;
    public:
        virtual void parseRawAST() override;
        YulIdentifierNode(nlohmann::json *rawAST);
};
};