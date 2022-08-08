#pragma once
#include <nlohmann/json.hpp>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include<llvm/IR/LLVMContext.h>

namespace yul2llvm {

class TranslateYulToLLVM {
    /// TODO: this is a placeholder interface, please replace this with the real
    /// interface

        //private fileds
        nlohmann::json yulAst;
        std::unique_ptr<llvm::LLVMContext> Context;
        std::unique_ptr<llvm::Module> Module;
        std::unique_ptr<llvm::IRBuilder<>> Builder;

        // private functions
        void initializeBuilder();
        int readJsonData(std::string filename);

    public: 
        TranslateYulToLLVM(std::string);
        void run(const nlohmann::json &yulAst);
};

} // namespace yul2llvm
