#include <iostream>
#include <libYulAST/YulContractNode.h>
#include <cassert>

using namespace yulast;

void YulContractNode::parseRawAST(const json *rawAST) {
    constructStruct();
    json::json_pointer p("/object_body/contract_body/children/0");
    json block = rawAST->at(p);
    assert(block["type"] ==  YUL_BLOCK_KEY);
    // add all functions
    for(auto &child : block["children"]){
        if(child["type"] == YUL_FUNCTION_DEFINITION_KEY){
            std::unique_ptr<YulFunctionDefinitionNode> f 
                = std::make_unique<YulFunctionDefinitionNode>(&child);
            functions.push_back(std::move(f));
        }
    }

    // add constructor

    p = "/contract_body/children/0"_json_pointer;
    block = rawAST->at(p);
    assert(block["type"] ==  YUL_BLOCK_KEY);
    // add all functions
    for(auto &child : block["children"]){
        if(child["type"] == YUL_FUNCTION_DEFINITION_KEY){
            std::unique_ptr<YulFunctionDefinitionNode> f 
                = std::make_unique<YulFunctionDefinitionNode>(&child);
            functions.push_back(std::move(f));
        }
    }


    //get contract name
    p = "/object_name/children/0"_json_pointer;
    contractName = rawAST->at(p);
    TheModule->setModuleIdentifier(contractName);
}

void YulContractNode::constructStruct(){    
    if(insertionOrder.size() == 0)
        return;
    std::vector<llvm::Type*> memberTypes;
    for(auto &field: insertionOrder){
        std::string typeStr = std::get<0>(typeMap[field]);
        int bitWidth = std::get<1>(typeMap[field]);
        llvm::Type* type = getType(bitWidth);
        memberTypes.push_back(type);
    }
    llvm::StructType *selfType = llvm::StructType::create(*TheContext, memberTypes, "struct_self");
    TheModule->getOrInsertGlobal("self", selfType);

}


void YulContractNode::buildTypeMap(const json &metadata){
    //for each storage location i.e. var
    for(auto &var: metadata["state_vars"]){
        std::string label = var["name"].get<std::string>();
        std::string typeStr = var["type"].get<std::string>();
        int bitWidth = metadata["types"][typeStr]["size"].get<int>()*8;
        typeMap[label] = std::make_tuple(typeStr, bitWidth);
        insertionOrder.push_back(label);
    }
}

YulContractNode::YulContractNode(const json *rawAST)
    : YulASTBase(rawAST,
                 YUL_AST_NODE_TYPE::YUL_AST_NODE_CONTRACT) {
    
    buildTypeMap(rawAST->at("metadata"));
    parseRawAST(rawAST);
    codegen(nullptr);
}

llvm::Value *YulContractNode::codegen(llvm::Function *enclosingFunction){
    for(auto &f: functions){
        f->codegen(nullptr);
    }
    return nullptr;
}

void YulContractNode::emitLoadIntrinsics(){
    // get result type;
    // get function type
    // create function
    // insert body
}

llvm::Type* YulContractNode::getType(int bitWidth){
    /**
     * @todo fix this for other types
     * 
     */
    bitWidth = 256; 
    return llvm::Type::getIntNTy(*TheContext, bitWidth);
}

std::vector<std::unique_ptr<YulFunctionDefinitionNode>>& YulContractNode::getFunctions(){
    return functions;
}


std::string YulContractNode::to_string() { return "contract"; }