#include<libYulAST/YulStatementNode.h>
#include<vector>
#include<nlohmann/json.hpp>

namespace yulast{
class YulBlockNode: protected YulStatementNode{
    protected:
        std::vector<YulStatementNode> statements;
    public:
        virtual void parseRawAST();
        YulBlockNode(nlohmann::json rawAST, YUL_AST_STATEMENT_NODE_TYPE);
};
};