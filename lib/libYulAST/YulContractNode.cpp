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

TypeInfo parseType(std::string_view type, const json &metadata) {
  assert(metadata.contains("types") && "Metadata does not contain types");
  auto &types = metadata["types"];
  assert(types.contains(type) && "Types does not contain the requested type");

  std::string typeStr = type.data();
  assert(types[typeStr].contains("kind") && "mapping kind not found in types");
  // starts with not available until c++20
  std::string arrayTypeLit("t_array");
  std::string mappingTypeLit("t_mapping");
  if (type.substr(0, arrayTypeLit.size()) == arrayTypeLit) {
    std::regex arrayTypeRegex("t_array\\((.*)\\)([0-9]+)_(storage|memory)?");
    std::smatch match;
    bool found = std::regex_search(typeStr, match, arrayTypeRegex);
    assert(found && "Array type pattern did not match");
    if (!found) {
      assert(false && "arry pattern did not match");
    }
    assert(types.contains(match[1].str()) &&
           "Array child type not found in types");
    return TypeInfo(types[type.data()]["kind"].get<std::string>(), // kind
                    "",                                            // keytype
                    match[1].str(),                                // valueType
                    types[typeStr]["size"].get<int>());            // size

  } else if (type.substr(0, mappingTypeLit.size()) == mappingTypeLit) {
    assert(types[typeStr].contains("key") && "mapping kind not found in types");
    assert(types[typeStr].contains("value") &&
           "mapping kind not found in types");
    std::string keyType = types[typeStr]["key"].get<std::string>();
    std::string valueType = types[typeStr]["value"].get<std::string>();
    assert(types.contains(keyType) &&
           "keyType not found in metadata for mapping type");
    assert(types.contains(valueType) &&
           "valueType not found in metadata for mapping type");
    return TypeInfo(types[type.data()]["kind"].get<std::string>(), // kind
                    keyType, valueType, -1);
  } else {
    // assume primitive type
    //@todo another branch will be added when we implement solidity structs
    assert(types[typeStr].contains("size") &&
           "Primitive type does not conatiain size");
    return TypeInfo(types[type.data()]["kind"].get<std::string>(), // kind
                    "", "", types[typeStr]["size"].get<int>());
  }
}

void YulContractNode::buildTypeInfoMap(const json &metadata) {
  for (auto &type : metadata["types"].items()) {
    std::string typeStr = type.key();
    typeInfoMap[typeStr] = parseType(typeStr, metadata);
  }
}

void YulContractNode::buildVarTypeMap(const json &metadata) {
  // for each storage location i.e. var
  for (auto &var : metadata["state_vars"]) {
    std::string varName = var["name"].get<std::string>();
    std::string varType = var["type"].get<std::string>();
    int offset = var["offset"].get<int>();
    int slot = var["slot"].get<int>();
    varTypeMap[varName] = {std::move(varType), slot, offset};
    structFieldOrder.push_back(varName);
  }
}

YulContractNode::YulContractNode(const json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_CONTRACT) {
  buildTypeInfoMap(rawAST->at("metadata"));
  buildVarTypeMap(rawAST->at("metadata"));
  parseRawAST(rawAST);
}

std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &
YulContractNode::getFunctions() {
  return functions;
}

std::string YulContractNode::to_string() { return "contract"; }

std::map<std::string, StorageVarInfo> &YulContractNode::getVarTypeMap() {
  return varTypeMap;
}

std::map<std::string, TypeInfo> &YulContractNode::getTypeInfoMap() {
  return typeInfoMap;
}

std::string YulContractNode::getStateVarNameBySlotOffset(int slot, int offset) {
  for (auto &f : structFieldOrder) {
    auto varEntry = varTypeMap[f];
    int varSlot = varEntry.slot;
    int varOffset = varEntry.offset;
    if (varOffset == offset && varSlot == slot) {
      return f;
    }
  }
  return "";
}

std::vector<std::string> &YulContractNode::getStructFieldOrder() {
  return structFieldOrder;
}