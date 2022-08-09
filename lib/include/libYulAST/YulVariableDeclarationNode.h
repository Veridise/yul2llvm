#include<libYulAST/YulStatementNode.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulExpressionNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulVariableDeclarationNode: protected YulStatementNode{
    protected:
        YulIdentifierNode variableName;
        YulExpressionNode value;
    public:
        virtual void parseRawAST();
        YulVariableDeclarationNode(nlohmann::json rawAST);
};
};