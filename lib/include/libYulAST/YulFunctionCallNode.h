#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<libYulAST/YulFunctionArgListNode.h>
#include<libnlohmann/json.hpp>

namespace yulast{
class YulFunctionCallNode: protected YulExpressionNode{
    protected:
        YulIdentifierNode callee;
        YulFunctionArgListNode argList;
    public:
        virtual void parseRawAST();
        YulFunctionCallNode(nlohmann::json rawAST, YUL_AST_EXPRESSION_TYPE);
};
};