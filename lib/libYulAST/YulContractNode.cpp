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

TypeInfo YulContractNode::parseType(std::string_view type, const json &metadata) {
  assert(metadata.contains("types") && "Metadata does not contain types");
  auto &types = metadata["types"];
  assert(types.contains(type) && "Types does not contain the requested type");

  std::string typeStr = type.data();
  assert(types[typeStr].contains("kind") && "mapping kind not found in types");
  // starts with not available until c++20
  std::string arrayTypeLit("t_array");
  std::string mappingTypeLit("t_mapping");
  std::string structTypeLit("t_struct");
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
    return TypeInfo(typeStr, types[type.data()]["kind"].get<std::string>(), // kind
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
    return TypeInfo(typeStr, types[type.data()]["kind"].get<std::string>(), // kind
                    keyType, valueType, -1);
  } else if (type.substr(0, structTypeLit.size()) == structTypeLit){
    assert(types[typeStr].contains("fields") && "fields not found in struct type");
    StructTypeResult res = patternMatcher.parseStructType(type);
    TypeInfo typeInfo(typeStr, "struct", "", "", res.size);
    for(auto &field: types[typeStr]["fields"]){
      TypeInfo ti = parseType(field["type"].get<std::string>(), metadata);
      typeInfo.members.push_back(StructField(field["name"].get<std::string>(), ti, 
                                            field["slot"].get<int>(), field["offset"].get<int>()));
    }
    structTypes[typeStr] = typeInfo;
    return typeInfo;
  } else {
    // assume primitive type
    assert(types[typeStr].contains("size") &&
           "Primitive type does not conatiain size");
    return TypeInfo(typeStr, types[type.data()]["kind"].get<std::string>(), // kind
                    "", "", types[typeStr]["size"].get<int>());
  }
}

void YulContractNode::buildTypeInfoMap(const json &metadata) {
  for (auto &type : metadata["types"].items()) {
    std::string typeStr = type.key();
    typeInfoMap[typeStr] = parseType(typeStr, metadata);
  }
}

void YulContractNode::buildStateVars(const json &metadata) {
  // for each storage location i.e. var
  //create self struct
  TypeInfo self("t_struct(self)", "struct", "", "", 0);
  for (auto &var : metadata["state_vars"]) {
    std::string varName = var["name"].get<std::string>();
    std::string varType = var["type"].get<std::string>();
    int offset = var["offset"].get<int>();
    int slot = var["slot"].get<int>();
    self.members.push_back(StructField(varName, typeInfoMap[varType], slot, offset));
  }
  structTypes["self"] = self;
}

YulContractNode::YulContractNode(const json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_CONTRACT) {
  buildTypeInfoMap(rawAST->at("metadata"));
  buildStateVars(rawAST->at("metadata"));
  parseRawAST(rawAST);
}

std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &
YulContractNode::getFunctions() {
  return functions;
}

std::string YulContractNode::to_string() { return "contract"; }


std::map<std::string, TypeInfo> &YulContractNode::getTypeInfoMap() {
  return typeInfoMap;
}

unsigned int YulContractNode::getFieldIndexInStruct(TypeInfo ti, std::string name){
  assert(ti.kind == "struct" && "type not struct");
  unsigned int i = 0;
  for(auto mem: ti.members){
    if(name == mem.name){
      return i;
    }
    i++;
  }
  assert(false && "field member not found");
  return INT_MAX;
}

std::vector<int> YulContractNode::getIndexPathByName(std::vector<std::string> namePath){
  std::vector<int> indices = {0};
  TypeInfo currentStruct = structTypes["self"];
  int index;
  for(auto name: namePath){
    index = getFieldIndexInStruct(currentStruct, name);
    indices.push_back(index);
    auto memType = currentStruct.members[index].typeInfo;
    if(memType.kind == "struct"){
      currentStruct = memType;
    } else {
      break;
    }
  }
  return indices;
}

std::vector<int> YulContractNode::getIndexPathBySlotOffset(int slot, int offset){
  auto namePath = getNamePathBySlotOffset(slot, offset);
  return getIndexPathByName(namePath);  
}

std::vector<std::string> YulContractNode::getNamePathBySlotOffset(int slot, int offset){
  return _getNamePathBySlotOffset(structTypes["self"], 0, 0, slot, offset);
}

std::vector<std::string> YulContractNode::_getNamePathBySlotOffset(TypeInfo type, int currentSlot, int currentOffset, 
                                                                              int slot, int offset) {
  std::vector<std::string> namePath;
  for(auto mem: type.members){
    if(mem.typeInfo.kind == "struct"){
      std::vector<std::string> subPath = _getNamePathBySlotOffset(mem.typeInfo, 
                                                              currentSlot+mem.slot, currentOffset+mem.offset,
                                                              slot, offset);
      if(subPath.size() > 0) {
        namePath.push_back(mem.name);
        namePath.insert(namePath.end(), subPath.begin(), subPath.end());
        return namePath;
      }
    }
    else if(mem.slot + currentSlot == slot && mem.offset +currentOffset == offset){
      namePath.push_back(mem.name);
    }
  }
  return namePath;
}


std::string_view YulContractNode::getName(){
  return contractName;
}

std::map<std::string, TypeInfo> &YulContractNode::getStructTypes(){
  return structTypes;
}
