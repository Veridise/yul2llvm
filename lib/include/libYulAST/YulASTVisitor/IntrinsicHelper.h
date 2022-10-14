#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulASTVisitor/CodegenConstants.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Value.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <regex>

using namespace yulast;
class YulIntrinsicHelper {
  LLVMCodegenVisitor &visitor;

public:
  // Helpers
  bool isFunctionCallIntrinsic(llvm::StringRef calleeName);
  bool skipDefinition(
      llvm::StringRef calleeName); // skip definition of functions that are
                                   // going to be replaced out
  llvm::Value *handleIntrinsicFunctionCall(YulFunctionCallNode &node);
  llvm::Value *getPointerToStorageVarByName(std::string,
                                            llvm::Instruction *insertPoint);
  llvm::StringRef getStorageVarYulTypeByName(llvm::StringRef name);
  llvm::Type *getTypeByTypeName(llvm::StringRef type);
  llvm::Function *getOrCreateFunction(std::string, llvm::FunctionType *);
  YulIntrinsicHelper(LLVMCodegenVisitor &v);
  llvm::Type *getReturnType(llvm::StringRef);
  llvm::SmallVector<llvm::Type *>
  getFunctionArgTypes(std::string calleeName,
                      llvm::SmallVector<llvm::Value *> &argsV);
  llvm::FunctionType *getFunctionType(YulFunctionCallNode &node,
                                      llvm::SmallVector<llvm::Value *> &argsV);

  /**
   * @brief Take any llvm value of arbitrary width return i256 values after
   * performing necessary bit masking.
   *
   * @param v value
   * @param type type to be read
   * @param bitwidth the width of the final datatype
   * @return llvm::Value*
   */
  llvm::Value *cleanup(llvm::Value *v, llvm::StringRef type, llvm::Value *offset, 
                       llvm::Instruction *insertionPoint = nullptr);

  // Emit intrinsics
  llvm::Value *handleMapIndex(YulFunctionCallNode &node);
  llvm::Value *handleMStoreFunctionCall(YulFunctionCallNode &node);
  llvm::Value *handleShl(YulFunctionCallNode &node);
  llvm::Value *handleAllocateUnbounded(YulFunctionCallNode &node);
  llvm::Value *handleArrayIndexAccess(YulFunctionCallNode &node);
  llvm::Value *handlePointerAdd(llvm::Value *v1, llvm::Value *v2);
  llvm::Value *handlePointerSub(llvm::Value *v1, llvm::Value *v2);
  llvm::Value *handleReadFromMemory(YulFunctionCallNode &node);
  llvm::Value *handleWriteToMemory(YulFunctionCallNode &node);
  llvm::Value *handleConvertRationalXByY(YulFunctionCallNode &node);
  llvm::Value *handleAnd(YulFunctionCallNode &node);
  llvm::Value *handleByte(YulFunctionCallNode &node);

  // Rewrites
  void rewriteIntrinsics(llvm::Function *enclosingFunction);
  void rewriteMapIndexCalls(llvm::CallInst *callInst);
  void rewriteStorageUpdateIntrinsic(llvm::CallInst *callInst);
  void rewriteStorageOffsetLoadIntrinsic(llvm::CallInst *callInst,
                                         std::smatch &match);
  void rewriteStorageDynamicLoadIntrinsic(llvm::CallInst *callInst,
                                          std::smatch &match);
  void rewriteStorageLoadIntrinsic(llvm::CallInst *callInst);
  void rewriteCallIntrinsic(llvm::CallInst *callInst);

  // Yul EVM functions
  llvm::Value *handleAddFunctionCall(YulFunctionCallNode &node);
  llvm::Value *handleSubFunctionCall(YulFunctionCallNode &node);

  LLVMCodegenVisitor &getVisitor();
};