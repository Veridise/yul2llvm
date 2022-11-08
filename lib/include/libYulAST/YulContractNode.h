#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <regex>
#include <vector>

namespace yulast {
struct TypeInfo {
public:
  std::string kind;
  std::string keyType;
  std::string valueType;
  int size;
  TypeInfo(std::string kind, std::string keyType, std::string valueType,
           int size)
      : kind(kind), keyType(keyType), valueType(valueType), size(size) {}
  TypeInfo() {}
};

struct StorageVarInfo {
public:
  std::string type;
  int slot;
  int offset;
  StorageVarInfo(std::string type, int slot, int offset)
      : type(type), slot(slot), offset(offset) {}
  StorageVarInfo() {}
};

class YulContractNode : public YulASTBase {
  // map from label -> (type name, bitwidth)
  // needed to maintain the index of a member in a struct
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
  std::string contractName;
  void buildVarTypeMap(const json &);
  void buildTypeInfoMap(const json &);
  void allocateSelfStruct();
  virtual void parseRawAST(const json *) override;
  std::map<std::string, StorageVarInfo> varTypeMap;
  std::map<std::string, TypeInfo> typeInfoMap;

public:
  YulContractNode(const json *);
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &getFunctions();

  std::map<std::string, StorageVarInfo> &getVarTypeMap();
  std::vector<std::string> &getInsertionOrder();
  std::string to_string() override;
  std::vector<std::string> structFieldOrder;
  std::vector<std::string> &getStructFieldOrder();
  std::string getStateVarNameBySlotOffset(int slot, int offset);
  std::map<std::string, TypeInfo> &getTypeInfoMap();
  std::string_view getName();
};

}; // namespace yulast