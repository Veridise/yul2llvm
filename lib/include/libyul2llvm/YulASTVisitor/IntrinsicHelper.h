#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/IntrinsicPatterns.h>
#include <libYulAST/YulFunctionCallNode.h>
#include <libyul2llvm/YulASTVisitor/CodegenConstants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Value.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <regex>

using namespace yulast;
class YulIntrinsicHelper {
  LLVMCodegenVisitor &visitor;
  /**
   * @brief Rewrite the the yul intrinsic for updating storage var by name.
   *
   * @param callInst The instruction to rewrite
   * @param name Name of the variable. Its a vector of strings that as the name
   * the rewitten variable could be a member of a struct
   * @param val The value to be stored at variable with the give name in @ref
   * param
   */
  void rewriteUpdateStorageVarByName(llvm::CallInst *callInst,
                                     std::vector<std::string> name,
                                     llvm::Value *val);

  /**
   * @brief Rewrite the the yul intrinsic for reading storage var by name.
   * @param callInst The instruction to rewrite
   * @param name Name of the variable. Its a vector of strings that as the name
   * the rewitten variable could be a member of a struct
   */
  void rewriteLoadStorageVarByName(llvm::CallInst *callInst,
                                   std::vector<std::string> name);
  void rewriteUpdateStorageByLocation(llvm::CallInst *callInst,
                                      llvm::Value *slot, llvm::Value *offset,
                                      llvm::Type *type, llvm::Value *val);
  void rewriteLoadStorageByLocation(llvm::CallInst *callInst, llvm::Value *slot,
                                    llvm::Value *offset, llvm::Type *type);
  llvm::Value *getPointerInSlotByOffset(llvm::CallInst *callInst,
                                        llvm::Value *slot, llvm::Value *offset);
  IntrinsicPatternMatcher patternMatcher;

  /**
   * @brief Get the llvm value name From Name Path object
   *
   * @param namePath
   * @return std::string
   */
  std::string getValueNameFromNamePath(std::vector<std::string> namePath);

public:
  // Helpers
  bool isFunctionCallIntrinsic(llvm::StringRef calleeName);
  bool skipDefinition(
      llvm::StringRef calleeName); // skip definition of functions that are
                                   // going to be replaced out
  llvm::Value *handleIntrinsicFunctionCall(YulFunctionCallNode &node);

  /**
   * @brief Get the Pointer To Storage Var By Name object
   * @param name The name of the storage variable being accessed.
   * Note: Name can be referring to member of a struct. Therefore
   * This vector is an array of such name components.
   * @param insertPoint
   * @return llvm::Value*
   */
  llvm::Value *getPointerToStorageVarByName(std::vector<std::string> name,
                                            llvm::Instruction *insertPoint);
  llvm::Type *getTypeByTypeName(llvm::StringRef type, const int addrSpaceId);
  llvm::Function *getOrCreateFunction(std::string, llvm::FunctionType *);
  YulIntrinsicHelper(LLVMCodegenVisitor &v);
  llvm::Type *getReturnType(llvm::StringRef);
  llvm::SmallVector<llvm::Type *>
  getFunctionArgTypes(std::string_view calleeName,
                      llvm::SmallVector<llvm::Value *> &argsV);
  llvm::FunctionType *getFunctionType(YulFunctionCallNode &node,
                                      llvm::SmallVector<llvm::Value *> &argsV);
  int foldAdds(llvm::BinaryOperator *inst, llvm::CallInst *callInst);
  std::string convertStructTypeFromYulToABI(std::string yulType);

  // Emit intrinsics
  llvm::Value *handleMapIndex(YulFunctionCallNode &node);
  llvm::Value *handleMStoreFunctionCall(YulFunctionCallNode &node);
  llvm::Value *handleShl(YulFunctionCallNode &node);
  llvm::Value *handleAllocateUnbounded(YulFunctionCallNode &node);
  llvm::Value *handleMemoryArrayIndexAccess(YulFunctionCallNode &node);
  llvm::Value *handlePointerAdd(llvm::Value *v1, llvm::Value *v2);
  llvm::Value *handlePointerSub(llvm::Value *v1, llvm::Value *v2);
  llvm::Value *handleReadFromMemory(YulFunctionCallNode &node);
  llvm::Value *handleWriteToMemory(YulFunctionCallNode &node);
  llvm::Value *handleConvertRationalXByY(YulFunctionCallNode &node);
  llvm::Value *handleAnd(YulFunctionCallNode &node);
  llvm::Value *handleByte(YulFunctionCallNode &node);
  llvm::Value *handleIsZero(YulFunctionCallNode &node);
  llvm::Value *handleCompare(YulFunctionCallNode &node,
                             llvm::ICmpInst::Predicate op);

  // Rewrites
  void rewriteIntrinsics(llvm::Function *enclosingFunction);
  void rewriteMapIndexCalls(llvm::CallInst *callInst);
  void rewriteStorageOffsetUpdateIntrinsic(llvm::CallInst *callInst, int offset,
                                           std::string srcTypeName,
                                           std::string destTypeName);
  void rewriteStorageDynamicUpdateIntrinsic(llvm::CallInst *callInst,
                                            std::string srcTypeName,
                                            std::string destTypeName);
  void rewriteStorageUpdateIntrinsic(llvm::CallInst *callInst);
  void rewriteStorageOffsetLoadIntrinsic(llvm::CallInst *callInst, int offset,
                                         std::string yulTypeStr);
  void rewriteStorageDynamicLoadIntrinsic(llvm::CallInst *callInst,
                                          std::string yulTypeStr);
  void rewriteStorageLoadIntrinsic(llvm::CallInst *callInst);
  void rewriteCallIntrinsic(llvm::CallInst *callInst);
  void rewriteStorageArrayIndexAccess(llvm::CallInst *callInst);
  void rewriteConvertStorageToStoragePtr(llvm::CallInst *callInst);
  void rewriteConvertStorageToMemoryPtr(llvm::CallInst *callInst,
                                        ConvertXToYResult res);
  void rewriteConvertXToY(llvm::CallInst *callInst);

  // Yul EVM functions
  llvm::Value *handleAddFunctionCall(YulFunctionCallNode &node);
  llvm::Value *handleDiv(YulFunctionCallNode &node);
  llvm::Value *handleMul(YulFunctionCallNode &node);
  llvm::Value *handleSubFunctionCall(YulFunctionCallNode &node);

  LLVMCodegenVisitor &getVisitor();
};