#pragma once

#include <libYulAST/YulASTBase.h>
#include <libYulAST/YulFunctionDefinitionNode.h>

namespace yulast {
class YulContractNode : public YulASTBase {
  // map from label -> (type name, bitwidth)
  // needed to maintain the index of a member in a struct
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> functions;
  std::string contractName;
  void buildTypeMap(const json &);
  void allocateSelfStruct();
  virtual void parseRawAST(const json *) override;
  void constructStruct();

public:
  YulContractNode(const json *);
  std::vector<std::unique_ptr<YulFunctionDefinitionNode>> &getFunctions();
  llvm::StringMap<std::tuple<std::string, int>> &getTypeMap();
  std::vector<std::string> &getInsertionOrder();
  std::string to_string() override;
  llvm::SmallVector<std::string> structFieldOrder;
  llvm::SmallVector<std::string>& getStructFieldOrder();
  llvm::StringMap<std::tuple<std::string, int>> typeMap;
};

}; // namespace yulast