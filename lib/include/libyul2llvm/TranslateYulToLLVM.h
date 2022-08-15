#pragma once
#include <nlohmann/json.hpp>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include<llvm/IR/LLVMContext.h>
#include<vector>
#include<libYulAST/YulFunctionDefinitionNode.h>

namespace yul2llvm {

class TranslateYulToLLVM {
    /// TODO: this is a placeholder interface, please replace this with the real
    /// interface

        //private fileds
        nlohmann::json rawAST;
        int readJsonData(std::string filename);
        void traverseJson(nlohmann::json);
        std::vector<yulast::YulFunctionDefinitionNode> functions;
        std::string inputFilename, outputFilename;
    public: 
        TranslateYulToLLVM(std::string inputFilename, std::string outputFilename = NULL);
        void run();
};

} // namespace yul2llvm
