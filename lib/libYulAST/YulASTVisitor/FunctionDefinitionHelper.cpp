#include <libYulAST/YulASTVisitor/FunctionDefinitionHelper.h>

llvm::Type *
YulFunctionDefinitionHelper::getReturnType(YulFunctionDefinitionNode &node) {
  llvm::Type *retType;
  if (node.getRets().size() != 0)
    retType = llvm::Type::getVoidTy(visitor.getContext());
  else
    retType = llvm::Type::getIntNTy(visitor.getContext(), 256);
  return retType;
}

std::vector<llvm::Type *> YulFunctionDefinitionHelper::getFunctionArgTypes(
    YulFunctionDefinitionNode &node) {
  int numargs = node.getArgs().size();
  std::vector<llvm::Type *> funcArgTypes(
      numargs, llvm::Type::getIntNTy(visitor.getContext(), 256));
  return funcArgTypes;
}

llvm::Function *YulFunctionDefinitionHelper::createPrototype(
    YulFunctionDefinitionNode &node,
    llvm::SmallVector<llvm::Attribute::AttrKind> &attrs) {
  int numargs;
  if (!node.hasArgs())
    numargs = 0;
  else
    numargs = node.getArgs().size();

  std::vector<llvm::Type *> funcArgTypes(
      numargs, llvm::Type::getIntNTy(visitor.getContext(), 256));

  llvm::Type *retType = getReturnType(node);

  /**
   * Note: remove old function declarations created by
   * yul_function_call nodes encountered before encountering
   * this yul_function_definition nodes
   */
  llvm::Function *oldFunction =
      visitor.getModule().getFunction(node.getName());

  if (oldFunction) {
    visitor.getModule().getFunctionList().remove(oldFunction);
  }

  llvm::FunctionType *FT = llvm::FunctionType::get(retType, funcArgTypes, false);

  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             node.getName(),
                             visitor.getModule());

  int idx = 0;
  for (auto &arg : F->args()) {
    arg.setName(node.getArgs().at(idx++)->getIdentfierValue());
  }
}

std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>>
YulFunctionDefinitionHelper::buildFunctionAttributes(
    YulFunctionDefinitionNode &node) {
  auto attributes =
      std::make_unique<llvm::SmallVector<llvm::Attribute::AttrKind>>();
  if (node.getName().substr(0, 7) == "revert_") {
    attributes->push_back(llvm::Attribute::NoReturn);
  }
  return attributes;
}

void YulFunctionDefinitionHelper::createVarsForArgsAndRets(YulFunctionDefinitionNode &node, llvm::Function *F) {
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(visitor.getContext(), "entry", F);
  visitor.getBuilder().SetInsertPoint(BB);

  if (node.hasArgs()) {
    for (auto &arg : node.getArgs()) {
      llvm::AllocaInst *a =
          visitor.CreateEntryBlockAlloca(F, arg->getIdentfierValue().append("_arg"));
      visitor.getNamedValuesMap()[arg->getIdentfierValue()] = a;
    }
  }

  for (auto &f : F->args()) {
    visitor.getBuilder().CreateStore(&f, visitor.getNamedValuesMap()[f.getName().str()]);
  }

  if (node.hasRets() != NULL) {
    for (auto &arg : node.getRets()) {
      llvm::AllocaInst *a = visitor.CreateEntryBlockAlloca(F, arg->getIdentfierValue());
      visitor.getNamedValuesMap()[arg->getIdentfierValue()] = a;
    }
  }
}

void YulFunctionDefinitionHelper::visitYulFunctionDefinitionNode(
    YulFunctionDefinitionNode &node) {
  llvm::Function *F, *enclosingFunction;
  enclosingFunction = visitor.getBuilder().GetInsertBlock()->getParent();
  llvm::Function *F = visitor.getModule().getFunction(node.getName());
  llvm::SmallVector<llvm::Attribute::AttrKind> attributes;
  if (!F)
    createPrototype(node, attributes);
  visitor.getNamedValuesMap().clear();
  createVarsForArgsAndRets(node, F);
  visitor.visit(node.getBody());
  if (!node.hasRets()) {
    visitor.getBuilder().CreateRetVoid();
  } else {
    // @todo assuming rets has only a single element
    llvm::Value *v = visitor.getBuilder().CreateLoad(
        llvm::Type::getIntNTy(visitor.getContext(), 256),
        visitor.getNamedValuesMap()[node.getRets()[0]->getIdentfierValue()]);
    visitor.getBuilder().CreateRet(v);
  }
}

