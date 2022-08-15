#pragma once
#include<nlohmann/json.hpp>
#include<libYulAST/YulConstants.h>
#include<llvm/IR/LLVMContext.h>
#include<llvm/IR/IRBuilder.h>
#include<llvm/IR/Module.h>


using json=nlohmann::json;

namespace yulast{
    class YulASTBase{
        protected:
        json *rawAST;
        YUL_AST_NODE_TYPE nodeType;     
    

        virtual void parseRawAST();

        public:
        static llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                          const std::string &VarName);
            static  std::unique_ptr<llvm::LLVMContext> TheContext;
            static std::unique_ptr<llvm::Module> TheModule;
            static std::unique_ptr<llvm::IRBuilder<>> Builder;
            static std::map<std::string, llvm::AllocaInst *> NamedValues;


            std::unique_ptr<llvm::LLVMContext> getContext();
            std::unique_ptr<llvm::Module> getModule();
            std::unique_ptr<llvm::IRBuilder<>> getBuilder();

            bool llvmInitialized=false;     
            virtual ~YulASTBase() {};
            virtual llvm::Value *codegen(llvm::Function *F);
            virtual std::string to_string();
            YulASTBase(json *rawAST, YUL_AST_NODE_TYPE nodeType);
            bool sanityCheckPassed(std::string);
    };
};