#include <libYulAST/YulASTVisitor/FunctionDeclaratorVisitor.h>
#include <libYulAST/YulASTVisitor/FunctionDefinitionHelper.h>
FunctionDeclaratorVisitor::FunctionDeclaratorVisitor(llvm::LLVMContext &c,
                                                     llvm::Module &m,
                                                     YulIntrinsicHelper &ih)
    : TheContext(c), TheModule(m), intrinsicHelper(ih) {}

llvm::Type *
FunctionDeclaratorVisitor::getReturnType(YulFunctionDefinitionNode &node) {
  llvm::Type *retType;
  if (!node.hasRets())
    retType = llvm::Type::getVoidTy(TheContext);
  else
    retType = llvm::Type::getIntNTy(TheContext, 256);
  return retType;
}

std::vector<llvm::Type *> FunctionDeclaratorVisitor::getFunctionArgTypes(
    YulFunctionDefinitionNode &node) {
  int numargs = node.getArgs().size();
  std::vector<llvm::Type *> funcArgTypes(
      numargs, llvm::Type::getIntNTy(TheContext, 256));
  return funcArgTypes;
}

std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>>
FunctionDeclaratorVisitor::buildFunctionAttributes(
    YulFunctionDefinitionNode &node) {
  auto attributes =
      std::make_unique<llvm::SmallVector<llvm::Attribute::AttrKind>>();
  if (node.getName().substr(0, 7) == "revert_") {
    attributes->push_back(llvm::Attribute::NoReturn);
  }
  return attributes;
}

void FunctionDeclaratorVisitor::visitYulContractNode(YulContractNode &node) {
  for (auto &f : node.getFunctions()) {
    visit(*f);
  }
}

void FunctionDeclaratorVisitor::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode &node) {
  if (intrinsicHelper.skipDefinition(node.getName()))
    return;
  int numargs = node.hasArgs() ? node.getArgs().size() : 0;
  std::vector<llvm::Type *> funcArgTypes(
      numargs, llvm::Type::getIntNTy(TheContext, 256));
  llvm::Type *retType = getReturnType(node);
  llvm::FunctionType *FT =
      llvm::FunctionType::get(retType, funcArgTypes, false);

  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, node.getName(), TheModule);

  int idx = 0;
  for (auto &arg : F->args()) {
    arg.setName(node.getArgs().at(idx++)->getIdentfierValue());
  }
}
