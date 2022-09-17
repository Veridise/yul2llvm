#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>

namespace yulast {
class YulContractNode : public YulASTBase {
  // map from label -> (type name, bitwidth)
  // needed to maintain the index of a member in a struct
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
  std::string contractName;
  void buildTypeMap(const json &);
  void allocateSelfStruct();
  virtual void parseRawAST(const json *) override;
  llvm::StringMap<std::tuple<std::string, int, int, int>> typeMap;

public:
  YulContractNode(const json *);
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &getFunctions();
  // map from var name to type, size(bitwidth), offset, slot
  llvm::StringMap<std::tuple<std::string, int, int, int>> &getTypeMap();
  std::vector<std::string> &getInsertionOrder();
  std::string to_string() override;
  llvm::SmallVector<std::string> structFieldOrder;
  llvm::SmallVector<std::string> &getStructFieldOrder();
  std::string getStateVarNameBySlotOffset(int slot, int offset);
};

}; // namespace yulast