#pragma once
#include<nlohmann/json.hpp>
#include<libYulAST/YulConstants.h>
using json=nlohmann::json;


namespace yulast{
    class YulASTBase{
        protected:
        json *rawAST;
        YUL_AST_NODE_TYPE nodeType;     
        bool parsed=false; 
        virtual void parseRawAST();
        public:
            virtual void codegen();
            virtual ~YulASTBase() {};
            virtual std::string to_string();

            YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodeType);
            bool isParsed();
            void setParsed(bool val);
            bool sanityCheckPassed(std::string);
    };
};