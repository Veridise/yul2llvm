#include <libYulAST/YulASTVisitor/FunctionDeclaratorVisitor.h>
#include <libYulAST/YulASTVisitor/FunctionDefinitionHelper.h>
FunctionDeclaratorVisitor::FunctionDeclaratorVisitor(llvm::LLVMContext &c,
                                                     llvm::Module &m,
                                                     YulIntrinsicHelper &ih)
    : TheContext(c), TheModule(m), intrinsicHelper(ih) {}

llvm::Type *
FunctionDeclaratorVisitor::getReturnType(YulFunctionDefinitionNode &node) {
  llvm::Type *retType;
  LLVMCodegenVisitor &visitor = intrinsicHelper.getVisitor();
  if (!node.hasRets())
    retType = llvm::Type::getVoidTy(TheContext);
  else {
    int numRets = node.getRets().size();
    if (numRets > 1) {

      llvm::SmallVector<llvm::Type *> typeMembers(
          numRets, llvm::Type::getIntNTy(visitor.getContext(), 256));
      llvm::StructType *newType = llvm::StructType::create(
          visitor.getContext(), typeMembers, node.getName() + "_type");
      visitor.getReturnTypesMap()[node.getName()] = newType;
      retType = newType->getPointerTo();
      llvm::Value *structVar = new llvm::GlobalVariable(
          visitor.getModule(), newType, false,
          llvm::GlobalValue::LinkageTypes::ExternalLinkage, nullptr,
          node.getName() + "_ret");
      visitor.getReturnStructs()[node.getName()] = structVar;
    } else
      retType = llvm::Type::getIntNTy(TheContext, 256);
  }
  return retType;
}

llvm::SmallVector<llvm::Type *> FunctionDeclaratorVisitor::getFunctionArgTypes(
    YulFunctionDefinitionNode &node) {
  int numargs = node.getNumArgs();
  // make first arg as self arg
  llvm::SmallVector<llvm::Type *> funcArgTypes = {
      llvm::Type::getIntNPtrTy(TheContext, 256)};
  llvm::SmallVector<llvm::Type *> origArgs(
      numargs, llvm::Type::getIntNTy(TheContext, 256));
  funcArgTypes.append(origArgs);
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
  llvm::SmallVector<llvm::Type *> funcArgTypes = getFunctionArgTypes(node);
  llvm::Type *retType = getReturnType(node);
  llvm::FunctionType *FT =
      llvm::FunctionType::get(retType, funcArgTypes, false);

  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, node.getName(), TheModule);
  int idx = 0;
  // atleast one argument (self) is guranteed to be present
  F->arg_begin()->setName("__self");
  for (auto arg = F->arg_begin() + 1; arg != F->arg_end(); arg++) {
    arg->setName(node.getArgs().at(idx++)->getIdentfierValue());
  }
  intrinsicHelper.getVisitor().definedFunctions[node.getName()] = F;
}
