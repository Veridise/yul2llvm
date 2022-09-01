#include <cassert>
#include <libYulAST/YulContractNode.h>

using namespace yulast;

void YulContractNode::parseRawAST(const json *rawAST) {
  // get contract name
  json::json_pointer p = "/object_name/children/0"_json_pointer;
  contractName = rawAST->at(p).get<std::string>();
  TheModule->setModuleIdentifier(contractName);

  constructStruct();
  p = "/object_body/contract_body/children/0"_json_pointer;
  json block = rawAST->at(p);
  assert(block["type"] == YUL_BLOCK_KEY);
  // add all functions
  for (auto &child : block["children"]) {
    if (child["type"] == YUL_FUNCTION_DEFINITION_KEY) {
      std::unique_ptr<YulFunctionDefinitionNode> f =
          std::make_unique<YulFunctionDefinitionNode>(&child);
      functions.push_back(std::move(f));
    }
  }

  // add constructor

  p = "/contract_body/children/0"_json_pointer;
  block = rawAST->at(p);
  assert(block["type"] == YUL_BLOCK_KEY);
  // add all functions
  for (auto &child : block["children"]) {
    if (child["type"] == YUL_FUNCTION_DEFINITION_KEY) {
      std::unique_ptr<YulFunctionDefinitionNode> f =
          std::make_unique<YulFunctionDefinitionNode>(&child);
      functions.push_back(std::move(f));
    }
  }
}

void YulContractNode::constructStruct() {
  if (structFieldOrder.size() == 0)
    return;
  std::vector<llvm::Type *> memberTypes;
  for (auto &field : structFieldOrder) {
    std::string typeStr = std::get<0>(typeMap[field]);
    int bitWidth = std::get<1>(typeMap[field]);
    llvm::Type *type = getType(bitWidth);
    memberTypes.push_back(type);
  }
  selfType = llvm::StructType::create(*TheContext, memberTypes, "self_type");
  llvm::Constant *init = llvm::Constant::getNullValue(selfType);
  self = new llvm::GlobalVariable(
      *TheModule, selfType, false,
      llvm::GlobalValue::LinkageTypes::ExternalLinkage, init, "__self");
}

void YulContractNode::buildTypeMap(const json &metadata) {
  // for each storage location i.e. var
  for (auto &var : metadata["state_vars"]) {
    std::string label = var["name"].get<std::string>();
    std::string typeStr = var["type"].get<std::string>();
    int bitWidth = metadata["types"][typeStr]["size"].get<int>() * 8;
    typeMap[label] = std::make_tuple(std::move(typeStr), bitWidth);
    structFieldOrder.push_back(std::move(label));
  }
}

YulContractNode::YulContractNode(const json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_CONTRACT) {

  buildTypeMap(rawAST->at("metadata"));
  parseRawAST(rawAST);
  codegen(nullptr);
}

llvm::Value *YulContractNode::codegen(llvm::Function *enclosingFunction) {
  for (auto &f : functions) {
    f->codegen(nullptr);
  }
  return nullptr;
}

llvm::Type *YulContractNode::getType(int bitWidth) {
  /**
   * @todo fix this for other types
   *
   */
  bitWidth = 256;
  return llvm::Type::getIntNTy(*TheContext, bitWidth);
}

std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &
YulContractNode::getFunctions() {
  return functions;
}

std::string YulContractNode::to_string() { return "contract"; }