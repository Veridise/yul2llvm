#pragma once
#include<nlohmann/json.hpp>
#include<libYulAST/YulConstants.h>
#include<llvm/IR/LLVMContext.h>
#include<llvm/IR/IRBuilder.h>
#include<llvm/IR/Module.h>


using json=nlohmann::json;

static std::unique_ptr<llvm::LLVMContext> TheContext = std::make_unique<llvm::LLVMContext>();
static std::unique_ptr<llvm::Module> TheModule =std::make_unique<llvm::Module>("yul2llvm-artihmetic-ops.ll", *TheContext);
static std::unique_ptr<llvm::IRBuilder<>> Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
static std::map<std::string, llvm::AllocaInst *> NamedValues;

namespace yulast{
    class YulASTBase{
        protected:
        json *rawAST;
        YUL_AST_NODE_TYPE nodeType;     
    

        virtual void parseRawAST();

        public:
            bool llvmInitialized=false;     
            virtual ~YulASTBase() {};
            virtual std::string to_string();

            YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodeType);
            bool sanityCheckPassed(std::string);
    };
};