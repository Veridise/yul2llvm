#include<libYulAST/YulASTBase.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulStatementNode: protected YulASTBase{
    protected:
        YUL_AST_STATEMENT_NODE_TYPE statementType;
    public:
        YulStatementNode(nlohmann::json *rawAST, YUL_AST_STATEMENT_NODE_TYPE statementType);
        virtual ~YulStatementNode();
};
};