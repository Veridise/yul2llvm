#pragma once
#include<nlohmann/json.hpp>
using json=nlohmann::json;

namespace yulast{
    enum YUL_AST_NODE_TYPE {
        UNDEFINED_TYPE,
        YUL_AST_NODE_STATEMENT,
        YUL_AST_NODE_FUNCTION_ARG_LIST,
        YUL_AST_NODE_FUNCTION_RET_LIST,
        YUL_AST_NODE_EXPRESSION,
        YUL_AST_NODE_TYPED_IDENTIFIER_LIST
    };

    enum YUL_AST_STATEMENT_NODE_TYPE {
        YUL_AST_STATEMENT_ASSIGNMENT,
        YUL_AST_STATEMENT_VARIABLE_DECLARATION,
        YUL_AST_STATEMENT_FUNCTION_DEFINITION,
        YUL_AST_STATEMENT_BLOCK, 
        YUL_AST_STATEMENT_EXPRESSION,
        YUL_AST_STATEMENT_LEAVE
    };

    enum YUL_AST_EXPRESSION_TYPE {
        YUL_AST_EXPRESSION_ADDITION,
        YUL_AST_EXPRESSION_FUNCTION_CALL, 
        YUL_AST_EXPRESSION_IDENTIFIER
    };

    class YulASTBase{
        protected:
        json *rawAST;
        YUL_AST_NODE_TYPE nodeType;      

        public:
            virtual void codegen();
            YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodeType);
            virtual void parseRawAST();
            virtual ~YulASTBase();
    };
};