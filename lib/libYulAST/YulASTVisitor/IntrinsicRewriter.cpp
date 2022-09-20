#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicEmitter.h>

llvm::SmallVector<llvm::CallInst *>
collectCalls(llvm::Function *enclosingFunction) {
  llvm::SmallVector<llvm::CallInst *> oldInstructions;
  for (auto b = enclosingFunction->begin(); b != enclosingFunction->end();
       b++) {
    for (auto i = b->begin(); i != b->end(); i++) {
      auto inst = llvm::dyn_cast<llvm::CallInst>(&(*i));
      if (inst) {
        oldInstructions.push_back(inst);
      }
    }
  }
  return oldInstructions;
}

void YulIntrinsicHelper::rewriteMapIndexCalls(llvm::CallInst *callInst) {
  llvm::SmallVector<llvm::Type *> argTypes;
  llvm::Type *retType = llvm::Type::getIntNPtrTy(visitor.getContext(), 256);
  argTypes.push_back(retType);
  argTypes.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
  llvm::FunctionType *FT = llvm::FunctionType::get(retType, argTypes, false);
  llvm::Function *mapIndexFunction = getOrCreateFunction("pyul_map_index", FT);
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  if (offset) {
    std::string varname = visitor.currentContract->getStateVarNameBySlotOffset(
        offset->getZExtValue(), 0);
    llvm::Value *mapPtr = getPointerToStorageVarByName(varname);
    llvm::Value *key = callInst->getArgOperand(1);
    llvm::SmallVector<llvm::Value *> args;
    args.push_back(mapPtr);
    args.push_back(key);
    llvm::CallInst *newCall =
        llvm::CallInst::Create(mapIndexFunction, args, "pyul_map_index");
    llvm::ReplaceInstWithInst(callInst, newCall);
  } else {
    llvm::SmallVector<llvm::Value *> args;
    for (auto &arg : callInst->args()) {
      args.push_back(arg);
    }
    llvm::CallInst *newCall =
        llvm::CallInst::Create(mapIndexFunction, args, "pyul_map_index");
    llvm::ReplaceInstWithInst(callInst, newCall);
  }
}

void YulIntrinsicHelper::rewriteStorageDynamicLoadIntrinsic(
    llvm::CallInst *callInst) {
  assert(callInst->getNumArgOperands() == 2 &&
         "Wrong number of arguments to storage load inst");
  auto slot = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(1));
  if (offset && slot) {
    std::string varname = visitor.currentContract->getStateVarNameBySlotOffset(
        slot->getZExtValue(), offset->getZExtValue());

    llvm::Value *ptr = getPointerToStorageVarByName(varname);
    int bitWidth = std::get<1>(visitor.currentContract->getTypeMap()[varname]);
    llvm::Type *loadType =
        llvm::Type::getIntNTy(visitor.getContext(), bitWidth);
    llvm::Align align =
        visitor.getModule().getDataLayout().getABITypeAlign(loadType);
    auto newInst = new llvm::LoadInst(loadType, ptr, "pyul_storage_var_load",
                                      false, align);
    llvm::ReplaceInstWithInst(callInst, newInst);
  } else {
    llvm::Type *loadType = llvm::Type::getIntNTy(visitor.getContext(), 256);
    llvm::Align align =
        visitor.getModule().getDataLayout().getABITypeAlign(loadType);
    auto newInst = new llvm::LoadInst(loadType, callInst->getArgOperand(0),
                                      "pyul_storage_var_load", false, align);
    llvm::ReplaceInstWithInst(callInst, newInst);
  }
}

void YulIntrinsicHelper::rewriteStorageUpdateIntrinsic(
    llvm::CallInst *callInst) {
  assert(callInst->getNumArgOperands() == 3 &&
         "Wrong number of arguments to storage store inst");
  callInst->setName("");
  llvm::Value *storeValue = callInst->getArgOperand(2);
  auto slot = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(1));
  if (offset && slot) {
    std::string varname = visitor.currentContract->getStateVarNameBySlotOffset(
        slot->getZExtValue(), offset->getZExtValue());

    llvm::Value *ptr = getPointerToStorageVarByName(varname);
    llvm::Align align = visitor.getModule().getDataLayout().getABITypeAlign(
        storeValue->getType());
    auto newInst = new llvm::StoreInst(storeValue, ptr, false, align);
    llvm::ReplaceInstWithInst(callInst, newInst);
  } else {
    llvm::Align align = visitor.getModule().getDataLayout().getABITypeAlign(
        storeValue->getType());
    auto newInst = new llvm::StoreInst(storeValue, callInst->getArgOperand(0),
                                       false, align);
    llvm::ReplaceInstWithInst(callInst, newInst);
  }
}

void YulIntrinsicHelper::rewriteIntrinsics(llvm::Function *enclosingFunction) {
  llvm::SmallVector<llvm::CallInst *> allCalls =
      collectCalls(enclosingFunction);
  for (llvm::CallInst *c : allCalls) {
    if (c->getCalledFunction()->getName() == "__pyul_map_index") {
      rewriteMapIndexCalls(c);
    } else if (c->getCalledFunction()->getName() ==
               "__pyul_storage_var_update") {
      rewriteStorageUpdateIntrinsic(c);
    } else if (c->getCalledFunction()->getName() ==
               "__pyul_storage_var_dynamic_load") {
      rewriteStorageDynamicLoadIntrinsic(c);
    }
  }
}