#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicHelper.h>
#include <libYulAST/YulASTVisitor/YulLLVMHelpers.h>

llvm::Type *checkAndGetPointeeType(llvm::Value *ptr) {
  llvm::PointerType *ptrSelfVarType =
      llvm::dyn_cast<llvm::PointerType>(ptr->getType());
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
void adjustCallReturns(llvm::CallInst*, llvm::Value *returnedVals, llvm::StructType*, 
                      LLVMCodegenVisitor &v);
llvm::Type* getExtCallReturnType(llvm::CallInst *callInst, LLVMCodegenVisitor &v, std::string name);
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
  retBuffer = visitor.CreateEntryBlockAlloca(visitor.currentFunction, 
                                            "ret_buffer",
                                            visitor.getDefaultType());
  retLen = callInst->getArgOperand(6);

  llvm::IRBuilder<> builder(visitor.getContext());
  builder.SetInsertPoint(callInst);
  llvm::Value *extCallCtx = getExtCallCtx(selector, gas, addr, value, retBuffer,
                                          retLen, visitor, builder);
  llvm::SmallVector<llvm::Value *> args;
  args.push_back(visitor.getSelfArg());
  args.push_back(extCallCtx);
  args.append(callArgs);
  llvm::Type *retType = getExtCallReturnType(callInst, visitor, selector);
  llvm::SmallVector<llvm::Type *> argtyps = getFunctionArgTypes("ext", args);
  llvm::FunctionType *callFT = llvm::FunctionType::get(
      retType->getPointerTo(), argtyps, false);
  llvm::Function *callF = getOrCreateFunction("pyul_call_" + selector, callFT);
  llvm::StructType *retStructType = llvm::dyn_cast<llvm::StructType>(retType);
  if(retStructType){
    llvm::CallInst *newCall = llvm::CallInst::Create(callF, args, "ret_struct", callInst);
    adjustCallReturns(callInst, newCall, retStructType, visitor);
  } else {
    llvm::CallInst *newCall = llvm::CallInst::Create(callF, args, "call_rv");    
    llvm::ReplaceInstWithInst(callInst, newCall);
  }
  removeOldCallArgs(callInst);
  
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

void YulIntrinsicHelper::rewriteUpdateStorageByLocation(llvm::CallInst *callInst,
                                                      llvm::Value *slot,
                                                      llvm::Value *offset,
                                                      llvm::Type *destType,
                                                      llvm::Value *storeValue) {
  llvm::Value *castedValue = llvm::CastInst::CreateIntegerCast(
      storeValue, destType, true, "casted_" + storeValue->getName(), callInst);
  llvm::Align align = visitor.getModule().getDataLayout().getABITypeAlign(
      castedValue->getType());
  llvm::Value *storageLocation =
      getPointerInSlotByOffset(callInst, slot, offset, destType);
  auto newInst =
      new llvm::StoreInst(castedValue, storageLocation, false, align);
  llvm::ReplaceInstWithInst(callInst, newInst);
}

void YulIntrinsicHelper::rewriteLoadStorageByLocation(llvm::CallInst *callInst,
                                                    llvm::Value *slot,
                                                    llvm::Value *offset,
                                                    llvm::Type *loadType) {
  llvm::Value *ptr = getPointerInSlotByOffset(callInst, slot, offset, loadType);
  llvm::Align align =
      visitor.getModule().getDataLayout().getABITypeAlign(loadType);
  auto loadedValue = new llvm::LoadInst(loadType, ptr, "pyul_storage_var_load",
                                        false, align, callInst);
  llvm::Instruction *casted = llvm::CastInst::CreateIntegerCast(
      loadedValue, visitor.getDefaultType(), true);
  llvm::ReplaceInstWithInst(callInst, casted);
}

void YulIntrinsicHelper::rewriteLoadStorageVarByName(llvm::CallInst *callInst,
                                               std::string varName) {
  llvm::Value *ptr = getPointerToStorageVarByName(varName, callInst);
  llvm::Type *selfVarType = checkAndGetPointeeType(ptr);
  llvm::Align align =
      visitor.getModule().getDataLayout().getABITypeAlign(selfVarType);

  llvm::LoadInst *loadInst = new llvm::LoadInst(
      selfVarType, ptr, "pyul_storage_var_load", false, align, callInst);

  llvm::CastInst *newInst = llvm::CastInst::CreateIntegerCast(
      loadInst, visitor.getDefaultType(), false, "i256_" + loadInst->getName());
  llvm::ReplaceInstWithInst(callInst, newInst);
}

void YulIntrinsicHelper::rewriteUpdateStorageVarByName(llvm::CallInst *callInst,
                                                 std::string varname,
                                                 llvm::Value *storeValue) {
  llvm::Value *ptr = getPointerToStorageVarByName(varname, callInst);
  llvm::Value *castedValue = llvm::CastInst::CreateIntegerCast(
      storeValue, ptr->getType()->getPointerElementType(), true,
      "casted_" + storeValue->getName(), callInst);
  llvm::Align align = visitor.getModule().getDataLayout().getABITypeAlign(
      castedValue->getType());
  auto newInst = new llvm::StoreInst(castedValue, ptr, false, align);
  llvm::ReplaceInstWithInst(callInst, newInst);
}

llvm::Value *YulIntrinsicHelper::getPointerInSlotByOffset(
    llvm::CallInst *callInst, llvm::Value *slot, llvm::Value *offset,
    llvm::Type *opType) {
  llvm::IRBuilder<> tempBuilder(callInst);
  int numElementsInSlot = SLOT_SIZE / opType->getIntegerBitWidth();
  if(!slot->getType()->isPointerTy()){
    slot = tempBuilder.CreateIntToPtr(slot, visitor.getDefaultType()->getPointerTo());
  }
  llvm::Constant *zeroConst = llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(visitor.getContext()), 0, 10);
  llvm::Value *offsetTrunc = tempBuilder.CreateIntCast(
      offset, llvm::Type::getInt32Ty(visitor.getContext()), false);
  llvm::ArrayType *slotArrayType =
      llvm::ArrayType::get(opType, numElementsInSlot);
  llvm::Value *arrayCastedSlot =
      tempBuilder.CreatePointerCast(slot, slotArrayType->getPointerTo());
  llvm::Value *location =
      tempBuilder.CreateGEP(slotArrayType, arrayCastedSlot,
                            {zeroConst, offsetTrunc}, "ptr_slot_offset");
  return location;
}

void YulIntrinsicHelper::rewriteStorageDynamicLoadIntrinsic(
    llvm::CallInst *callInst, std::smatch &match) {
  // R"(^read_from_storage_split_dynamic_(.*))"
  auto tempBuilder = llvm::IRBuilder<>(callInst);
  auto slot = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(1));
  std::string yulTypeStr = match[1].str();
  if (offset && slot) {
    std::string varname = visitor.currentContract->getStateVarNameBySlotOffset(
        slot->getZExtValue(), offset->getZExtValue());
    rewriteLoadStorageVarByName(callInst, varname);
  } else {
    rewriteLoadStorageByLocation(callInst, callInst->getArgOperand(0),
                               callInst->getArgOperand(1),
                               getTypeByTypeName(yulTypeStr));
  }
}

void YulIntrinsicHelper::rewriteStorageOffsetLoadIntrinsic(
    llvm::CallInst *callInst, std::smatch &match) {

  // R"(^read_from_storage(_split)?_offset_([0-9]+)_(.*)$)"
  std::string offsetStr = match[2];
  int offset;
  assert(offsetStr != "" && "offset not found in read_from_storage");
  std::string yulTypeStr = match[3];
  // Extract offset and load data type.
  if (llvm::StringRef(offsetStr).getAsInteger(10, offset)) {
    //@todo raise runtime error
    assert(false && "Cannot parse offset in read_from_storage");
  }
  std::string typeStr = match[3].str();
  int slot;
  if (llvm::ConstantInt *slotConst =
          llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0))) {
    slot = slotConst->getZExtValue();
    std::string varname =
        visitor.currentContract->getStateVarNameBySlotOffset(slot, offset);
    rewriteLoadStorageVarByName(callInst, varname);
  } else {
    llvm::Type *loadType = getTypeByTypeName(yulTypeStr);
    llvm::Constant *offsetConst =
        llvm::ConstantInt::get(visitor.getDefaultType(), offset, 10);
    rewriteLoadStorageByLocation(callInst, callInst->getArgOperand(0),
                               offsetConst, loadType);
  }
}

void YulIntrinsicHelper::rewriteStorageLoadIntrinsic(llvm::CallInst *callInst) {
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  std::regex readFromStorageOffsetRegex(
      R"(^read_from_storage(_split)?_offset_([0-9]+)_(.*)$)");
  std::regex readFromStorageDynamicRegex(
      R"(^read_from_storage_split_dynamic_(.*))");
  std::smatch match;
  std::string typeStr;
  if (std::regex_match(calleeName, match, readFromStorageOffsetRegex)) {
    rewriteStorageOffsetLoadIntrinsic(callInst, match);
  } else if (std::regex_match(calleeName, match, readFromStorageDynamicRegex)) {
    rewriteStorageDynamicLoadIntrinsic(callInst, match);
  } else {
    //@todo raise runtime error
    assert(false && "unrecognized read from storage dynamic intrinsic");
  }
}

void YulIntrinsicHelper::rewriteStorageUpdateIntrinsic(llvm::CallInst *callInst){
   std::regex updateValueOffsetRegex(
      R"(^update_storage_value_offset_([0-9]+)(.*)_to_(.*)$)");
   std::regex updateValueDynamicRegex(
      R"(^update_storage_value_(.*)_to_(.*)$)");
  std::smatch match;
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  if(std::regex_match(calleeName, match, updateValueOffsetRegex)){
    rewriteStorageOffsetUpdateIntrinsic(callInst, match);
  } else if(std::regex_match(calleeName, match, updateValueDynamicRegex)){
    rewriteStorageDynamicUpdateIntrinsic(callInst, match);
  } else {
    // @todo raise runtime error
    assert(false && "update_storage_value did not match any regex");
  }


}
void YulIntrinsicHelper::rewriteStorageDynamicUpdateIntrinsic(llvm::CallInst *callInst,
                                          std::smatch &match){
    llvm::IRBuilder<> tempBuilder(callInst);
  assert(callInst->getNumArgOperands() == 4 &&
          "Wrong number of arguments to dynamic storage store inst");
  callInst->setName("");
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  llvm::Value *slotArg = callInst->getArgOperand(1);
  llvm::Value *offsetArg = callInst->getArgOperand(2);
  llvm::Value *storeValue = callInst->getArgOperand(3);
  std::string srcTypeName = match[1].str();
  std::string destTypeName = match[2].str();
  auto slotConst = llvm::dyn_cast<llvm::ConstantInt>(slotArg);
  auto offsetConst = llvm::dyn_cast<llvm::ConstantInt>(offsetArg);
  if (slotConst && offsetConst) {
    std::string varname =
        visitor.currentContract->getStateVarNameBySlotOffset(
            slotConst->getZExtValue(), offsetConst->getZExtValue());
    rewriteUpdateStorageVarByName(callInst, varname, storeValue);
  } else {
    llvm::Type *destType = getTypeByTypeName(destTypeName);
    llvm::Value *offsetIndex = tempBuilder.CreateIntCast(offsetArg,
      llvm::Type::getInt32Ty(visitor.getContext()), false, "array_offset");
    rewriteUpdateStorageByLocation(callInst, slotArg,
                                  offsetIndex, destType, storeValue);
  }

}

// truncate and write
void YulIntrinsicHelper::rewriteStorageOffsetUpdateIntrinsic(
    llvm::CallInst *callInst, std::smatch &match) {
  llvm::IRBuilder<> tempBuilder(callInst);
  assert(callInst->getNumArgOperands() == 3 &&
          "Wrong number of arguments to storage store inst");
  callInst->setName("");
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  llvm::Value *storeValue = callInst->getArgOperand(2);
  std::string srcTypeName = match[2].str();
  std::string destTypeName = match[3].str();
  std::string offsetStr = match[1].str();
  int offset;
  if (llvm::StringRef(offsetStr).getAsInteger(10, offset)) {
    //@todo raise runtime error
    assert(false && "could not parse offset in update");
  }

  if (auto slot =
          llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(1))) {
    std::string varname =
        visitor.currentContract->getStateVarNameBySlotOffset(
            slot->getZExtValue(), offset);
    rewriteUpdateStorageVarByName(callInst, varname, storeValue);
  } else {
    llvm::Type *destType = getTypeByTypeName(destTypeName);
    llvm::Constant *offsetConst = llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(visitor.getContext()), offset, 10);
    rewriteUpdateStorageByLocation(callInst, callInst->getArgOperand(0),
                                  offsetConst, destType, storeValue);
  }

  
}


void YulIntrinsicHelper::rewriteStorageArrayIndexAccess(llvm::CallInst *callInst){
  assert(callInst->getNumArgOperands() == 3 && "Wrong number of args in storage_array_index access");
  llvm::IRBuilder<> builder(callInst);
  std::regex storageArrayIndexRegex(R"(^storage_array_index_access_t_array\$_(.*)_\$(\d+)_storage$)");
  std::string name = callInst->getCalledFunction()->getName().str();
  std::smatch match;
  if(!std::regex_match(name, match, storageArrayIndexRegex)){
    //@todo raise runtime error
    assert(false && "didnt match storage array index regex");
  }
  std::string elementTypeName = match[1].str();
  std::string sizeStr = match[2].str();
  int size;
  if(llvm::StringRef(sizeStr).getAsInteger(10, size)){
    //@todo raise runtime error
    assert(false && "could not parse array size");
  }
  llvm::Type *elementType = getTypeByTypeName(elementTypeName);
  llvm::Value *slot = callInst->getArgOperand(1);
  llvm::Value *index = callInst->getArgOperand(2);
  assert(index->getType()->isIntegerTy() && "index is not int type");
  llvm::Value *truncIndex = builder.CreateIntCast(index, llvm::Type::getInt32Ty(visitor.getContext()), false, "index32");
  std::string indexString;
  auto offsetConst = llvm::dyn_cast<llvm::ConstantInt>(truncIndex);
  if(offsetConst){
    indexString = std::to_string(offsetConst->getZExtValue());
  } else {
    indexString = truncIndex->getName();
  }
  auto slotConst = llvm::dyn_cast<llvm::ConstantInt>(slot);
  llvm::Value *arrayPtr;
  
  if(slotConst){
    std::string arrayName = visitor.currentContract->getStateVarNameBySlotOffset(slotConst->getZExtValue(), 0);
    arrayPtr = getPointerToStorageVarByName(arrayName, callInst);
  } else {
    if(slot->getType()->isPointerTy())
      arrayPtr = builder.CreatePointerCast(slot, visitor.getDefaultType()->getPointerTo(), slot->getName()+"_casted");
    else
      arrayPtr = builder.CreateIntToPtr(slot, visitor.getDefaultType()->getPointerTo(), slot->getName()+"_casted");
  }
  llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(visitor.getContext()),
                                                              0, 10);
  arrayPtr = builder.CreateLoad(visitor.getDefaultType()->getPointerTo(), arrayPtr, arrayPtr->getName().drop_front(4));
  llvm::Type *arrayType = llvm::ArrayType::get(elementType, 0);
  auto castedArrayPtr = builder.CreatePointerCast(arrayPtr, arrayType->getPointerTo());
  llvm::Value *elementPtr = builder.CreateGEP(arrayType, castedArrayPtr, {zero, truncIndex}, 
                              "ptr_"+arrayPtr->getName()+"["+indexString+"]");
  // Now we have computed the pointer to return. But yul expectes two return values. 
  // These values are packed in a struct. These will be unpacked in a parent AST node that uses this struct
  llvm::Value *scalarizedElementPtr = builder.CreatePtrToInt(elementPtr, visitor.getDefaultType());
  llvm::Value *zero256 = llvm::ConstantInt::get(visitor.getDefaultType(),
                                                              0, 10);
                                                          
  llvm::Value *structPtr = visitor.packRetsInStruct(name, {scalarizedElementPtr, zero256}, callInst);
  llvm::BasicBlock::InstListType &instList = callInst->getParent()->getInstList();
  llvm::BasicBlock::iterator callInstIt = callInst->getIterator();
  llvm::ReplaceInstWithValue(instList, callInstIt, structPtr);
}

void YulIntrinsicHelper::rewriteIntrinsics(llvm::Function *enclosingFunction) {
  llvm::SmallVector<llvm::CallInst *> allCalls =
      collectCalls(enclosingFunction);
  for (llvm::CallInst *c : allCalls) {
    if (c->getCalledFunction()->getName() == "__pyul_map_index") {
      rewriteMapIndexCalls(c);
    } else if (c->getCalledFunction()->getName().startswith(
                   "update_storage_value")) {
      rewriteStorageUpdateIntrinsic(c);
    } else if (c->getCalledFunction()->getName().startswith(
                   "read_from_storage")) {
      rewriteStorageLoadIntrinsic(c);
    } else if (c->getCalledFunction()->getName() == "call") {
      rewriteCallIntrinsic(c);
    } else if (c->getCalledFunction()->getName().startswith("storage_array_index_access_t_array")) {
      rewriteStorageArrayIndexAccess(c);
    }
  }
}