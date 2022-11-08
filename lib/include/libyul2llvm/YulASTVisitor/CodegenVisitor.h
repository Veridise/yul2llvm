#pragma once
class YulFunctionCallHelper;
class YulFunctionDefinitionHelper;
#include <libyul2llvm/YulASTVisitor/FunctionCallHelper.h>
#include <libyul2llvm/YulASTVisitor/FunctionDeclaratorVisitor.h>
#include <libyul2llvm/YulASTVisitor/FunctionDefinitionHelper.h>
#include <libyul2llvm/YulASTVisitor/IntrinsicHelper.h>
#include <libyul2llvm/YulASTVisitor/VisitorBase.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Base64.h>
#include <llvm/Support/SHA1.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>
#include <stack>

class LLVMCodegenVisitor : public YulASTVisitorBase {
private:
protected:
  YulIntrinsicHelper intrinsicHelper;
  std::unique_ptr<llvm::LLVMContext> TheContext;
  std::unique_ptr<llvm::Module> TheModule;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  // This is a map from <variable-name> -> llvm allocation location
  llvm::StringMap<llvm::AllocaInst *> NamedValues;
  // This is a map from <string-literal> -> global variable location
  llvm::StringMap<std::string> stringLiteralNames;
  llvm::StringMap<llvm::StructType *> returnTypes;
  llvm::StringMap<llvm::Value *> returnStructs;

  // data structures for self
  llvm::StructType *selfType;
  llvm::Value *ptrSelfPointer;

  llvm::Type *getTypeByInfo(llvm::StringRef typeStr,
                            std::map<std::string, TypeInfo> &typeInfoMap);
  void constructSelfStructType(YulContractNode &node);

  // external call context
  /**
   * context for external call
   * struct extCallCtx{
   *  llvm::Value *gas,
   *  llvm::Value *address,
   *  llvm::Value *value,
   *  llvm::Value *buffer,
   *  llvm::Value *retLen
   * }
   *
   */
  llvm::StructType *extCallCtxType;
  std::unique_ptr<YulFunctionCallHelper> funCallHelper;
  std::unique_ptr<YulFunctionDefinitionHelper> funDefHelper;
  void codeGenForOneVarAllocation(YulIdentifierNode &id, llvm::Type *);
  void runFunctionDeclaratorVisitor(YulContractNode &node);
  std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;
  void connectToBasicBlock(llvm::BasicBlock *nextBlock);
  llvm::Function *allocateStorageFunction;
  llvm::Function *allocateMemoryFunction;



  // helpers

public:
  YulContractNode *currentContract;
  llvm::Function *currentFunction;
  llvm::StringMap<llvm::Function *> definedFunctions;

  virtual void visitYulAssignmentNode(YulAssignmentNode &) override;
  virtual void visitYulBlockNode(YulBlockNode &) override;
  virtual void visitYulBreakNode(YulBreakNode &) override;
  virtual void visitYulCaseNode(YulCaseNode &) override;
  virtual void visitYulContinueNode(YulContinueNode &) override;
  virtual void visitYulContractNode(YulContractNode &) override;
  virtual void visitYulDefaultNode(YulDefaultNode &) override;
  virtual void visitYulForNode(YulForNode &) override;
  virtual llvm::Value *visitYulFunctionCallNode(YulFunctionCallNode &) override;
  virtual void
  visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &) override;
  virtual llvm::Value *visitYulIdentifierNode(YulIdentifierNode &) override;
  virtual void visitYulIfNode(YulIfNode &) override;
  virtual void visitYulLeaveNode(YulLeaveNode &) override;
  virtual llvm::Value *
  visitYulNumberLiteralNode(YulNumberLiteralNode &) override;
  virtual llvm::Value *
  visitYulStringLiteralNode(YulStringLiteralNode &) override;
  virtual void visitYulSwitchNode(YulSwitchNode &) override;
  virtual void
  visitYulVariableDeclarationNode(YulVariableDeclarationNode &) override;

  LLVMCodegenVisitor();
  virtual ~LLVMCodegenVisitor() {}

  // LLVM datastructures
  llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                           const std::string_view VarName,
                                           llvm::Type *type = nullptr);
  llvm::GlobalVariable *CreateGlobalStringLiteral(std::string literalValue,
                                                  std::string literalName);

  llvm::Module &getModule();
  llvm::IRBuilder<> &getBuilder();
  llvm::LLVMContext &getContext();
  llvm::legacy::FunctionPassManager &getFPM();
  llvm::StringMap<llvm::AllocaInst *> &getNamedValuesMap();
  llvm::StructType *getSelfType() const;
  llvm::StructType *getExtCallCtxType();
  llvm::SmallVector<llvm::Value *>
  getLLVMValueVector(llvm::ArrayRef<int> rawIndices);

  llvm::Value *getSelfArg() const;

  std::stack<std::tuple<llvm::BasicBlock *, llvm::BasicBlock *>>
      loopControlFlowBlocks;

  llvm::Type *getDefaultType() const;

  void dump(llvm::raw_ostream &os) const;
  void dumpToStdout() const;

  llvm::StringMap<llvm::StructType *> &getReturnTypesMap();
  llvm::StringMap<llvm::Value *> &getReturnStructs();
  llvm::Value *packRetsInStruct(llvm::StringRef functionName,
                                llvm::ArrayRef<llvm::Value *> rets,
                                llvm::Instruction *insertPoint);
  llvm::SmallVector<llvm::Value *>
  unpackFunctionCallReturns(YulExpressionNode &rhsExpression);
  YulIntrinsicHelper &getYulIntrisicHelper();

  llvm::Function *getAllocateStorageFunction();
  llvm::Function *getAllocateMemoryFunction();
  void setSelfPointer(llvm::Value *);
  llvm::Value *getSelfPointer();


};