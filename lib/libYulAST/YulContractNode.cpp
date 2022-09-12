#include <cassert>
#include <libYulAST/YulContractNode.h>

using namespace yulast;

void YulContractNode::parseRawAST(const json *rawAST) {
  // get contract name
  json::json_pointer p = "/object_name/children/0"_json_pointer;
  contractName = rawAST->at(p).get<std::string>();
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
}

std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &
YulContractNode::getFunctions() {
  return functions;
}

std::string YulContractNode::to_string() { return "contract"; }

llvm::StringMap<std::tuple<std::string, int>> &YulContractNode::getTypeMap() {
  return typeMap;
}

llvm::SmallVector<std::string> &YulContractNode::getStructFieldOrder() {
  return structFieldOrder;
}