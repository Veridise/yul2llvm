#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <regex>


#define TYPEINFO_KIND(x) llvm::StringRef(std::get<0>(x))
#define TYPEINFO_KEY_TYPE(x) llvm::StringRef(std::get<1>(x))
#define TYPEINFO_VALUE_TYPE(x) llvm::StringRef(std::get<2>(x))
#define TYPEINFO_SIZE(x) std::get<3>(x)

namespace yulast {
//                            kind        , key type, value type, size
using TypeInfo = std::tuple<std::string, std::string, std::string, int>;
  // map from var name to            typestr,  slot, offset
using StorageVarInfo = std::tuple<std::string, int, int>;
class YulContractNode : public YulASTBase {
  // map from label -> (type name, bitwidth)
  // needed to maintain the index of a member in a struct
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
  std::string contractName;
  void buildVarTypeMap(const json &);
  void buildTypeInfoMap(const json &);
  void allocateSelfStruct();
  virtual void parseRawAST(const json *) override;
  llvm::StringMap<StorageVarInfo> varTypeMap;
  llvm::StringMap<TypeInfo> typeInfoMap;

public:
  YulContractNode(const json *);
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &getFunctions();

  llvm::StringMap<StorageVarInfo> &getVarTypeMap();
  std::vector<std::string> &getInsertionOrder();
  std::string to_string() override;
  llvm::SmallVector<std::string> structFieldOrder;
  llvm::SmallVector<std::string> &getStructFieldOrder();
  std::string getStateVarNameBySlotOffset(int slot, int offset);
  llvm::StringMap<TypeInfo> &getTypeInfoMap();

};

}; // namespace yulast