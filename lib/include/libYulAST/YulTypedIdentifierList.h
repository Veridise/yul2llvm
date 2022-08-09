#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulIdentifierNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulTypedIdentifierList: protected YulASTBase{
    std::vector<YulIdentifierNode> identifierList;
    public:
        virtual void parseRawAST();
        YulTypedIdentifierList(nlohmann::json rawAST);
};
};