#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>

namespace yulast {
class YulContractNode : public YulASTBase{
    // map from label -> (type name, bitwidth)
    std::map<std::string, std::tuple<std::string, int>> typeMap;
    // needed to maintain the index of a member in a struct
    std::vector<std::string> insertionOrder;
    std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
    std::string contractName;
    void buildTypeMap(const json &) ;
    void allocateSelfStruct();
    virtual void parseRawAST(const json*) override;
    llvm::Type* getType(int bitwidt);
    void constructStruct();
    void emitLoadIntrinsics();
    public:
        llvm::Value *codegen(llvm::Function *enclosingFunction) override;
        YulContractNode(const json *);
        std::vector<std::unique_ptr<YulFunctionDefinitionNode>>& getFunctions();
        std::map<std::string, std::string>& getTypeMap();
        std::vector<std::string>& getInsertionOrder();
        std::string to_string() override;
};

};