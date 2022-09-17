#pragma once
#include <libYulAST/YulASTVisitor/VisitorBase.h>
#include <llvm/IR/Attributes.h>
using namespace yulast;
class FunctionDeclaratorVisitor : public YulASTVisitorBase {
public:
  llvm::LLVMContext &TheContext;
  llvm::Module &TheModule;

  FunctionDeclaratorVisitor(llvm::LLVMContext &c, llvm::Module &m);
  virtual void visitYulContractNode(YulContractNode &node) override;

  virtual void
  visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &node) override;

  llvm::Type *getReturnType(YulFunctionDefinitionNode &node);

  std::vector<llvm::Type *>
  getFunctionArgTypes(YulFunctionDefinitionNode &node);

  std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>>
  buildFunctionAttributes(YulFunctionDefinitionNode &node);
};