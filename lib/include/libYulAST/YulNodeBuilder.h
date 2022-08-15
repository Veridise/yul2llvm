#pragma once

#include<libYulAST/YulASTBase.h>
#include<libYulAST/YulAssignmentNode.h>
#include<libYulAST/YulVariableDeclarationNode.h>
#include<libYulAST/YulLeaveNode.h>
#include<nlohmann/json.hpp>

#include<libYulAST/YulExpressionNode.h>
#include<libYulAST/YulFunctionCallNode.h>
#include<libYulAST/YulIdentifierNode.h>

namespace yulast{
class YulStatementBuilder{
    public:
        static YulStatementNode* Builder(json *rawAST);

};

class YulExpressionBuilder{
    public:
        static YulExpressionNode* Builder(json *rawAST);
};
};
