#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicHelper.h>
#include <libYulAST/YulASTVisitor/YulLLVMHelpers.h>

llvm::Type *checkAndGetPointeeType(llvm::Value* ptr){
  llvm::PointerType *ptrSelfVarType = llvm::dyn_cast<llvm::PointerType>(ptr->getType());
  assert(ptrSelfVarType && "map field type is not a pointer");
  return ptrSelfVarType->getElementType();
  
}

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

llvm::Value *getAddress(llvm::Value *);
std::string getSelector(llvm::Value *);
llvm::SmallVector<llvm::Value *> decodeArgsAndCleanup(llvm::Value *);
llvm::Value *getExtCallCtx(llvm::StringRef selector, llvm::Value *gas,
                           llvm::Value *address, llvm::Value *value,
                           llvm::Value *retBuffer, llvm::Value *retLen,
                           LLVMCodegenVisitor &v, llvm::IRBuilder<> &);
void removeOldCallArgs(llvm::CallInst *callInst);
/**
 * @brief The let _7 := call(gas(), expr_14_address,  0,  _5, sub(_6, _5), _5,
 * 32) yul statement is going to be rewritten into call fun_<selector>(self,
 * args)
 *
 * The gas and the value is going to go into a call-info struct in the self
 * struct.
 *
 * @param callInst
 */
void YulIntrinsicHelper::rewriteCallIntrinsic(llvm::CallInst *callInst) {
  assert(callInst->getNumArgOperands() == 7 &&
         "Wrong number of args to call intrinsic");
  llvm::Value *gas, *addr, *value, *retBuffer, *retLen;
  std::string selector;
  llvm::SmallVector<llvm::Value *> callArgs;
  gas = callInst->getArgOperand(0);
  addr = getAddress(callInst->getArgOperand(1));
  value = callInst->getArgOperand(2);
  selector = getSelector(callInst->getArgOperand(3));
  callArgs = decodeArgsAndCleanup(callInst->getArgOperand(4));
  retBuffer = callInst->getArgOperand(5);
  retLen = callInst->getArgOperand(6);

  llvm::IRBuilder<> builder(visitor.getContext());
  builder.SetInsertPoint(callInst);
  llvm::Value *extCallCtx = getExtCallCtx(selector, gas, addr, value, retBuffer,
                                          retLen, visitor, builder);

  llvm::SmallVector<llvm::Value *> args;

  args.push_back(visitor.getSelfArg());
  args.push_back(extCallCtx);
  args.append(callArgs);
  llvm::SmallVector<llvm::Type *> argtyps = getFunctionArgTypes("ext", args);
  llvm::FunctionType *callFT = llvm::FunctionType::get(
      llvm::Type::getIntNTy(visitor.getContext(), 256), argtyps, false);
  llvm::Function *callF = getOrCreateFunction("ext_fun_" + selector, callFT);

  llvm::CallInst *newCall = llvm::CallInst::Create(callF, args, "call_rv");
  removeOldCallArgs(callInst);
  llvm::ReplaceInstWithInst(callInst, newCall);
}

void YulIntrinsicHelper::rewriteMapIndexCalls(llvm::CallInst *callInst) {
  llvm::SmallVector<llvm::Type *> argTypes;
  auto tmpBuilder = llvm::IRBuilder<>(callInst);
  llvm::Type *retType = llvm::Type::getIntNPtrTy(visitor.getContext(), 256);
  argTypes.push_back(retType);
  argTypes.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
  llvm::FunctionType *FT = llvm::FunctionType::get(retType, argTypes, false);
  llvm::Function *mapIndexFunction = getOrCreateFunction("pyul_map_index", FT);
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  if (offset) {
    std::string varname = visitor.currentContract->getStateVarNameBySlotOffset(
        offset->getZExtValue(), 0);
    llvm::Value *ptrSelfVar = getPointerToStorageVarByName(varname, callInst);
    llvm::Type *mapType = checkAndGetPointeeType(ptrSelfVar); 
    llvm::Value *mapPtr = tmpBuilder.CreateLoad(mapType, ptrSelfVar, varname);
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
  // llvm::IRBuilder<> tmp(callInst);
  if (offset && slot) {
    std::string varname = visitor.currentContract->getStateVarNameBySlotOffset(
        slot->getZExtValue(), offset->getZExtValue());

    llvm::Value *ptr = getPointerToStorageVarByName(varname, callInst);
    llvm::Type *selfVarType = checkAndGetPointeeType(ptr);
    llvm::Align align =
        visitor.getModule().getDataLayout().getABITypeAlign(selfVarType);
    
    
    llvm::LoadInst *loadInst = new llvm::LoadInst(selfVarType, ptr, "pyul_storage_var_load",
                                      false, align, callInst);
    llvm::CastInst *newInst =  llvm::CastInst::CreateIntegerCast(loadInst, 
                                                      visitor.getDefaultType(), false, 
                                                      "i256_"+loadInst->getName()
                                                      );
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

    llvm::Value *ptr = getPointerToStorageVarByName(varname, callInst);
    ptr = llvm::CastInst::CreateBitOrPointerCast(ptr, 
                                        visitor.getDefaultType()->getPointerTo(), 
                                        "casted_"+ptr->getName(),
                                        callInst);
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
    } else if (c->getCalledFunction()->getName() == "call") {
      rewriteCallIntrinsic(c);
    }
  }
}