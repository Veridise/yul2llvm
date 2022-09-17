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
    int bitWidth;
    if (metadata["types"][typeStr].contains("size"))
      bitWidth = metadata["types"][typeStr]["size"].get<int>() * 8;
    else {
      // confirm that type is mapping
      if (typeStr.substr(0, 9) == "t_mapping") {
        // hold symboilc bitwidth because this is going to be an
        // llvm implementation detail
        bitWidth = 0;
      } else
        bitWidth = 256;
    }
    int offset = var["offset"].get<int>();
    int slot = var["slot"].get<int>();
    typeMap[label] =
        std::make_tuple(std::move(typeStr), bitWidth, slot, offset);
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

llvm::StringMap<std::tuple<std::string, int, int, int>> &
YulContractNode::getTypeMap() {
  return typeMap;
}

std::string YulContractNode::getStateVarNameBySlotOffset(int slot, int offset) {
  for (auto &f : structFieldOrder) {
    auto varEntry = typeMap[f];
    int varSlot = std::get<2>(varEntry);
    int varOffset = std::get<3>(varEntry);
    if (varOffset == offset && varSlot == slot) {
      return f;
    }
  }
  return "";
}

llvm::SmallVector<std::string> &YulContractNode::getStructFieldOrder() {
  return structFieldOrder;
}