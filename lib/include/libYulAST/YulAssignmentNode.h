#include<libYulAST/YulStatementNode.h>
#include<libYulAST/YulExpressionNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulAssignmentNode: protected YulStatementNode{
    protected:
        YulExpressionNode *lhs;
        YulExpressionNode *rhs;
    public:
        virtual void parseRawAST() override;
        YulAssignmentNode(nlohmann::json *rawAst);
};
};