#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<nlohmann/json.hpp>
#include<vector>

namespace yulast{
class YulFunctionArgListNode: protected YulASTBase{
    protected:
        std::vector<YulIdentifierNode> identifierNode;

    public:
        virtual void parseRawAST();
        YulFunctionArgListNode(nlohmann::json rawAST, YUL_AST_NODE_TYPE);
};
};