#include<libYulAST/YulStatementNode.h>
#include<nlohmann/json.hpp>

namespace yulast{
class YulLeaveNode: protected YulStatementNode{
    public:
        virtual void parseRawAST();
        YulLeaveNode(nlohmann::json rawAST);
};
};