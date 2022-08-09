#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionArgListNode.h>
#include<libYulAST/YulFunctionRetListNode.h>
#include<libYulAST/YulBlockNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulFunctionDefinitionNode: protected YulStatementNode{
    protected:
        YulIdentifierNode functionName;
        YulFunctionArgListNode args;
        YulFunctionRetListNode rets;
        YulBlockNode body;
    public:
    virtual void parseRawAST();
        YulFunctionDefinitionNode(nlohmann::json rawAST);
};
};