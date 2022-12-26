#include <cassert>
#include <libYulAST/YulContractNode.h>
#include <iostream>
#include <limits>
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

void YulContractNode::addPrimitiveTypes(){
  for(int i=1; i<=32; i++){
    std::string typeStr = "t_bytes"+std::to_string(i);
    TypeInfo ti(typeStr,
                    "", // kind
                    "", "", i);
    typeInfoMap[typeStr] = ti;
  }
  for(int i=1; i<=32; i++){
    std::string typeStr = "t_uint"+std::to_string(i*8);
    TypeInfo ti(typeStr,
                    "", // kind
                    "", "", i);
    typeInfoMap[typeStr] = ti;
  }
  for(int i=1; i<=32; i++){
    std::string typeStr = "t_int"+std::to_string(i*8);
    TypeInfo ti(typeStr,
                    "", // kind
                    "", "", i);
    typeInfoMap[typeStr] = ti;
  }
}

TypeInfo YulContractNode::parseType(std::string_view type,
                                    const json &metadata) {
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
    return TypeInfo(typeStr,
                    types[type.data()]["kind"].get<std::string>(), // kind
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
    return TypeInfo(typeStr,
                    types[type.data()]["kind"].get<std::string>(), // kind
                    keyType, valueType, -1);
  } else if (type.substr(0, structTypeLit.size()) == structTypeLit) {
    assert(types[typeStr].contains("fields") &&
           "fields not found in struct type");
    StructTypeResult res = patternMatcher.parseStructTypeFromStorageLayout(type);
    TypeInfo typeInfo(typeStr, "struct", "", "", res.size);
    int currentOffset = 0;
    int fieldIdx = 0;
    for (auto &field : types[typeStr]["fields"]) {
      TypeInfo ti = parseType(field["type"].get<std::string>(), metadata);
      StructField sf(field["name"].get<std::string>(),
                                             ti, field["slot"].get<int>(),
                                             field["offset"].get<int>());
      typeInfo.members.push_back(sf);
      typeInfo.offset2fieldIdx[currentOffset] = fieldIdx;
      typeInfo.prettyName=res.name;
      fieldIdx++;
      currentOffset += ti.size;
    }
    structTypes[res.name] = typeInfo;
    return typeInfo;
  } else {
    // assume unhandled primitive type and add it
    assert(types[typeStr].contains("size") &&
           "Primitive type does not conatiain size");
    return TypeInfo(typeStr,
                    types[type.data()]["kind"].get<std::string>(), // kind
                    "", "", types[typeStr]["size"].get<int>());
  }
}

void YulContractNode::buildTypeInfoMap(const json &metadata) {
  addPrimitiveTypes();
  for (auto &type : metadata["types"].items()) {
    std::string typeStr = type.key();
    typeInfoMap[typeStr] = parseType(typeStr, metadata);
  }
}

bool YulContractNode::parseStructFromAbiArg(const json &arg, std::string name, TypeInfo &ti){
  int size = 0;
  std::string structTypeLit = "struct ";
  std::string uintTypeLit = "uint";
  std::string intTypeLit = "int";
  int fieldIdx = 0;
  for(auto &comp: arg.at("components")){
    std::string internalTypeName = comp.at("internalType").get<std::string>();
    std::string abiTypeName;
    TypeInfo fieldTi;
    if(internalTypeName.substr(0, structTypeLit.size()) == structTypeLit){
      StructTypeResult res = patternMatcher.parseStructTypeFromAbi(internalTypeName);
      auto it = structTypes.find(res.name);
      if(it != structTypes.end()){
        fieldTi = it->second;
      } else {
        parseStructFromAbiArg(comp, res.name, fieldTi);
      }
    } else if(internalTypeName.substr(0, uintTypeLit.size()) == uintTypeLit ||
              internalTypeName.substr(0, intTypeLit.size()) == intTypeLit){
        std::string typeName = "t_"+internalTypeName;
        auto it = typeInfoMap.find(typeName);
        if(it == typeInfoMap.end()){
          assert(false && "Internal type not found when parsing struct from abi");
        }
        fieldTi = typeInfoMap[typeName];
    } else {
      assert(false && "Unhandled type found while parsing struct from abi function signatures");
    }
    std::string fieldName = arg.at("name").get<std::string>();
    StructField sf(fieldName, fieldTi, 0, 0);
    ti.members.push_back(sf);
    ti.offset2fieldIdx[size] = fieldIdx;
    fieldIdx++;
    size += fieldTi.size;
  }
  ti.size = size;
  ti.kind = "struct";
  ti.typeStr = "t_struct("+name+")"+std::to_string(size);
  ti.prettyName=name;
  typeInfoMap[ti.typeStr] = ti;
  structTypes[name] = ti;
  return true;
}

void YulContractNode::augmentTypeInfoMapFromAbi(const json &abi){
  std::string structTypeLit = "struct ";
  for(auto &fun: abi){
    std::deque<const json*> allStructs;
    for (auto &inp: fun.at("inputs")){
      std::string_view internalType = inp.at("internalType").get<std::string>();
      if(internalType.substr(0, structTypeLit.size()) ==
                  structTypeLit){
        StructTypeResult res = patternMatcher.parseStructTypeFromAbi(internalType);
        auto it = structTypes.find(res.name);
        if(it == structTypes.end()){
          TypeInfo ti;
          if(!parseStructFromAbiArg(inp, res.name, ti)) {
            assert(false && "Could not parse Struct");
          }
        }
      }
    }
    for (auto &op: fun.at("outputs")){
      std::string_view internalType = op.at("internalType").get<std::string>();
      if(internalType.substr(0, structTypeLit.size()) ==
                  structTypeLit){
        StructTypeResult res = patternMatcher.parseStructTypeFromAbi(internalType);
        auto it = structTypes.find(res.name);
        TypeInfo ti;
        if(parseStructFromAbiArg(op, res.name, ti)){
          assert(false && "Could not parse Struct");
        }
      }
    }
  }
}

void YulContractNode::buildStateVars(const json &metadata) {
  // for each storage location i.e. var
  // create self struct
  TypeInfo self("t_struct(self)", "struct", "", "", 0);
  self.prettyName = "self";
  int currentOffset = 0;
  int fieldIdx = 0;
  for (auto &var : metadata["state_vars"]) {
    std::string varName = var["name"].get<std::string>();
    std::string varType = var["type"].get<std::string>();
    int offset = var["offset"].get<int>();
    int slot = var["slot"].get<int>();
    TypeInfo ti = typeInfoMap[varType];
    StructField sf(varName, ti, slot, offset);
    self.members.push_back(sf);
    self.offset2fieldIdx[currentOffset] = fieldIdx;
    fieldIdx++;
    currentOffset += ti.size;
  }
  structTypes["self"] = self;
}

YulContractNode::YulContractNode(const json *rawAST)
    : YulASTBase(rawAST, YUL_AST_NODE_TYPE::YUL_AST_NODE_CONTRACT) {
  buildTypeInfoMap(rawAST->at("metadata"));
  // buildFunctionSignatures(rawAST->at("abi"));
  buildStateVars(rawAST->at("metadata"));
  augmentTypeInfoMapFromAbi(rawAST->at("abi"));
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

unsigned int YulContractNode::getFieldIndexInStruct(TypeInfo ti,
                                                    std::string name) {
  assert(ti.kind == "struct" && "type not struct");
  unsigned int i = 0;
  for (auto mem : ti.members) {
    if (name == mem.name) {
      return i;
    }
    i++;
  }

  auto it = std::find_if(ti.members.begin(), ti.members.end(),
                         [&](auto &mem) { return name == mem.name; });
  if (it != ti.members.end()) {
    return std::distance(ti.members.begin(), it);
  } else {
    assert(false && "field member not found");
    return std::numeric_limits<int>::max();
  }
}

std::vector<int>
YulContractNode::getIndexPathByName(std::vector<std::string> namePath) {
  std::vector<int> indices = {0};
  TypeInfo currentStruct = getSelfType();
  int index;
  for (auto &name : namePath) {
    index = getFieldIndexInStruct(currentStruct, name);
    indices.push_back(index);
    auto memType = currentStruct.members[index].typeInfo;
    if (memType.kind == "struct") {
      currentStruct = memType;
    } else {
      break;
    }
  }
  return indices;
}

std::vector<int> YulContractNode::getIndexPathBySlotOffset(int slot,
                                                           int offset) {
  auto namePath = getIdentifierDerefBySlotOffset(slot, offset);
  return getIndexPathByName(namePath);
}

std::vector<std::string>
YulContractNode::getIdentifierDerefBySlotOffset(int slot, int offset) {
  return _getNamePathBySlotOffset(getSelfType(), 0, 0, slot, offset);
}

std::vector<std::string> YulContractNode::_getNamePathBySlotOffset(
    TypeInfo type, int currentSlot, int currentOffset, int slot, int offset) {
  std::vector<std::string> namePath;
  for (auto mem : type.members) {
    if (mem.typeInfo.kind == "struct") {
      std::vector<std::string> subPath =
          _getNamePathBySlotOffset(mem.typeInfo, currentSlot + mem.slot,
                                   currentOffset + mem.offset, slot, offset);
      if (subPath.size() > 0) {
        namePath.push_back(mem.name);
        namePath.insert(namePath.end(), subPath.begin(), subPath.end());
        return namePath;
      }
    } else if (mem.slot + currentSlot == slot &&
               mem.offset + currentOffset == offset) {
      namePath.push_back(mem.name);
    }
  }
  return namePath;
}

std::string_view YulContractNode::getName() { return contractName; }

std::map<std::string, TypeInfo> &YulContractNode::getStructTypes() {
  return structTypes;
}

TypeInfo YulContractNode::getSelfType() {
  auto it = structTypes.find("self");
  assert(it != structTypes.end() && "self type not found in contract");
  return structTypes["self"];
}

void YulContractNode::buildFunctionSignatures(const json &abi) {
  for(auto &fun: abi){
    FunctionSignature funSig;
    funSig.name = fun["name"];
    for(auto &input: fun["inputs"]) {
      std::string typeName = input.at("internalType");
      int dotIdx = typeName.find(".");
      if(dotIdx != std::string::npos){
        std::string structName = typeName.substr(dotIdx+1, typeName.length()-dotIdx);
        auto it = structTypes.find(structName);
        assert(it != structTypes.end() && "Could not find struct in structTypes");
        if(it != structTypes.end())
          funSig.arguments.push_back(structTypes[structName]);
        else 
          std::cout<<structName<<" not found";
      } else {
        TypeInfo type;
        type.typeStr=typeName;
        funSig.arguments.push_back(type);
      }
    }
    std::vector<TypeInfo> rets;
    for(auto &output: fun["outputs"]) {
      std::string typeName = output.at("internalType");
      int dotIdx = typeName.find(".");
      if(dotIdx != std::string::npos){
        std::string structName = typeName.substr(dotIdx+1, typeName.length()-dotIdx);
        funSig.returns.push_back(structTypes[structName]);
      } else {
        TypeInfo type;
        type.typeStr=typeName;
        funSig.returns.push_back(type);
      }
    }
    functionSignatures[funSig.name] = funSig;
  }
}