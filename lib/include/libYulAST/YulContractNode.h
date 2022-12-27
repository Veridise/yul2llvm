#pragma once

#include <libYulAST/IntrinsicPatterns.h>
#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <regex>
#include <vector>

namespace yulast {

struct StructField;
/**
 * @brief
 *
 */
struct TypeInfo {
  /**
   * @brief Name of the type in the abi
   *
   */
  std::string typeStr;
  /**
   * @brief Helps identify if the type is a primitive, mapping or a struct
   *
   */
  std::string kind;
  /**
   * @brief This is valid only when kind is mapping. Helps identify type
   * of key and value
   *
   */
  std::string keyType;
  std::string valueType;
  /**
   * @brief Valid only when kind is struct
   * Refers to members of filed
   *
   */
  /**
   * @brief Name of the struct in the contract
   */

  std::string prettyName;
  std::vector<StructField> members;
  int size;
  std::map<int, int> offset2fieldIdx;
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

struct FunctionSignature {
  std::string name;
  std::vector<TypeInfo> arguments;
  std::vector<TypeInfo> returns;
};

class YulContractNode : public YulASTBase {
  // map from label -> (type name, bitwidth)
  // needed to maintain the index of a member in a struct
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
  std::string contractName;
  std::map<std::string, TypeInfo> typeInfoMap;
  IntrinsicPatternMatcher patternMatcher;
  std::map<std::string, TypeInfo> structTypes;
  std::map<std::string, FunctionSignature> functionSignatures;

  void buildTypeInfoMap(const json &);
  void augmentTypeInfoMapFromAbi(const json &);
  virtual void parseRawAST(const json *) override;
  void allocateSelfStruct();
  TypeInfo parseType(std::string_view type, const json &metadata);
  void buildStateVars(const nlohmann::json &metadata);
  unsigned int getFieldIndexInStruct(TypeInfo ti, std::string name);
  std::vector<std::string>
  _getNamePathBySlotOffset(TypeInfo type, int currentSlot, int currentOffset,
                           unsigned int slot, unsigned int offset);
  void buildFunctionSignatures(const json &);
  void addPrimitiveTypes();
  bool parseStructFromAbiArg(const json &arg, std::string name, TypeInfo &ti);
  TypeInfo getAbiComponentType(const json &component);
  void buildTypeFromAbiComponent(const json &component);

public:
  YulContractNode(const json *);
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &getFunctions();
  std::vector<std::string> &getInsertionOrder();
  std::string to_string() override;
  std::vector<std::string> getIdentifierDerefBySlotOffset(int slot, int offset);
  std::vector<int> getIndexPathBySlotOffset(int slot, int offset);
  std::vector<int> getIndexPathByName(std::vector<std::string>);
  std::map<std::string, TypeInfo> &getTypeInfoMap();
  std::string_view getName();
  std::map<std::string, TypeInfo> &getStructTypes();
  TypeInfo getSelfType();
};

}; // namespace yulast