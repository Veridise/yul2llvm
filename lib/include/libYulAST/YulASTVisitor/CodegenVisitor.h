#pragma once
class YulFunctionCallHelper;
class YulFunctionDefinitionHelper;
#include <libYulAST/YulASTVisitor/FunctionCallHelper.h>
#include <libYulAST/YulASTVisitor/FunctionDeclaratorVisitor.h>
#include <libYulAST/YulASTVisitor/FunctionDefinitionHelper.h>
#include <libYulAST/YulASTVisitor/IntrinsicHelper.h>
#include <libYulAST/YulASTVisitor/VisitorBase.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Base64.h>
#include <llvm/Support/SHA1.h>
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

  // data structures for self
  llvm::GlobalVariable *self;
  llvm::StructType *selfType;
  llvm::Type *getTypeByBitwidth(int bitWidth);
  void constructStruct(YulContractNode &node);

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
  void constructExtCallCtxType();

  std::unique_ptr<YulFunctionCallHelper> funCallHelper;
  std::unique_ptr<YulFunctionDefinitionHelper> funDefHelper;
  void codeGenForOneVarDeclaration(YulIdentifierNode &id, llvm::Type *);
  void runFunctionDeclaratorVisitor(YulContractNode &node);
  std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;
  void connectToBasicBlock(llvm::BasicBlock *nextBlock);

  // helpers

public:
  YulContractNode *currentContract;
  llvm::Function *currentFunction;
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
                                           const std::string &VarName,
                                           llvm::Type *type = nullptr);
  llvm::GlobalVariable *CreateGlobalStringLiteral(std::string literalValue,
                                                  std::string literalName);

  llvm::Module &getModule();
  llvm::IRBuilder<> &getBuilder();
  llvm::LLVMContext &getContext();
  llvm::legacy::FunctionPassManager &getFPM();
  llvm::StringMap<llvm::AllocaInst *> &getNamedValuesMap();
  llvm::GlobalVariable *getSelf() const;
  llvm::StructType *getSelfType() const;
  llvm::StructType *getExtCallCtxType();
  llvm::SmallVector<llvm::Value *>
  getLLVMValueVector(llvm::ArrayRef<int> rawIndices);

  std::stack<std::tuple<llvm::BasicBlock *, llvm::BasicBlock *>>
      loopControlFlowBlocks;

  void dump(llvm::raw_ostream &os) const;
  void dumpToStdout() const;
};