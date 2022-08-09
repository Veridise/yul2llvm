#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<vector>
#include<nlohmann/json.hpp>

namespace yulast{
class YulFunctionRetListNode: protected YulASTBase{
    protected:
        std::vector<YulIdentifierNode> identifierList;
    public:
        virtual void parseRawAST();
        YulFunctionRetListNode(nlohmann::json rawAST);
};
};