#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <regex>


namespace yulast {
struct TypeInfo { 
  public:
  std::string kind;
  std::string keyType;
  std::string valueType;
  int size;
  TypeInfo(std::string kind, std::string keyType, std::string valueType, int size):
    kind(kind), keyType(keyType), valueType(valueType), size(size){}
  TypeInfo(){}
    
};

struct StorageVarInfo{
  public:
  std::string type;
  int slot;
  int offset;
  StorageVarInfo(std::string type, int slot, int offset) :
    type(type), slot(slot), offset(offset){}
  StorageVarInfo(){}
  
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