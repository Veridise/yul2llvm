#pragma once

#include <libYulAST/IntrinsicPatterns.h>
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <regex>
#include <vector>

namespace yulast {

struct StructField;
struct TypeInfo {
  std::string typeStr;
  std::string kind;
  std::string keyType;
  std::string valueType;
  std::vector<StructField> members;
  int size;
  TypeInfo(std::string typeStr, std::string kind, std::string keyType,
           std::string valueType, int size)
      : typeStr(typeStr), kind(kind), keyType(keyType), valueType(valueType),
        size(size) {}
  TypeInfo() {}
};

struct StructField {
  std::string name;
  TypeInfo typeInfo;
  unsigned int slot;
  unsigned int offset;
  StructField(std::string name, TypeInfo ti, int slot, int offset)
      : name(name), typeInfo(ti), slot(slot), offset(offset) {}
};

class YulContractNode : public YulASTBase {
  // map from label -> (type name, bitwidth)
  // needed to maintain the index of a member in a struct
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
  std::string contractName;
  std::map<std::string, TypeInfo> typeInfoMap;
  IntrinsicPatternMatcher patternMatcher;
  std::map<std::string, TypeInfo> structTypes;

  void buildTypeInfoMap(const json &);
  virtual void parseRawAST(const json *) override;
  void allocateSelfStruct();
  TypeInfo parseType(std::string_view type, const json &metadata);
  void buildStateVars(const nlohmann::json &metadata);
  unsigned int getFieldIndexInStruct(TypeInfo ti, std::string name);
  std::vector<std::string> _getNamePathBySlotOffset(TypeInfo type,
                                                    int currentSlot,
                                                    int currentOffset, int slot,
                                                    int offset);

public:
  YulContractNode(const json *);
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &getFunctions();
  std::vector<std::string> &getInsertionOrder();
  std::string to_string() override;
  std::vector<std::string> getNamePathBySlotOffset(int slot, int offset);
  std::vector<int> getIndexPathBySlotOffset(int slot, int offset);
  std::vector<int> getIndexPathByName(std::vector<std::string>);
  std::map<std::string, TypeInfo> &getTypeInfoMap();
  std::string_view getName();
  std::map<std::string, TypeInfo> &getStructTypes();
};

}; // namespace yulast