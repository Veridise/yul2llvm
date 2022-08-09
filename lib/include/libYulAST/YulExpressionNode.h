#include<libYulAST/YulASTBase.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulExpressionNode: protected YulASTBase{
    protected:
        YUL_AST_EXPRESSION_TYPE expressionType;
    public:
        YulExpressionNode(nlohmann::json *rawAST, YUL_AST_EXPRESSION_TYPE exprType);
        virtual ~YulExpressionNode();
};
};