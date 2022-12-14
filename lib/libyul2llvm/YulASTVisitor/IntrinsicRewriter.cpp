#include "CallGenHelpers.h"
#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>
#include <libyul2llvm/YulASTVisitor/IntrinsicHelper.h>
#include <libyul2llvm/YulASTVisitor/YulLLVMHelpers.h>

llvm::Type *checkAndGetPointeeType(llvm::Value *ptr) {
  llvm::PointerType *ptrSelfVarType =
      llvm::dyn_cast<llvm::PointerType>(ptr->getType());
  assert(ptrSelfVarType && "map field type is not a pointer");
  return ptrSelfVarType->getElementType();
}
const static llvm::SmallVector<std::string> supportedIntrinsics(
    {"__pyul_map_index", "update_storage_value", "read_from_storage", "call",
     "storage_array_index_access_t_array", "convert_t_"});

llvm::SmallVector<llvm::CallInst *>
collectCalls(llvm::Function *enclosingFunction) {
  llvm::SmallVector<llvm::CallInst *> oldInstructions;
  for (auto b = enclosingFunction->begin(); b != enclosingFunction->end();
       b++) {
    for (auto i = b->begin(); i != b->end(); i++) {
      auto inst = llvm::dyn_cast<llvm::CallInst>(&(*i));
      if (inst) {
        for (const std::string &candidate : supportedIntrinsics) {
          if (inst->getCalledFunction()->getName().startswith(candidate))
            oldInstructions.push_back(inst);
        }
      }
    }
  }
  return oldInstructions;
}

int YulIntrinsicHelper::foldAdds(llvm::BinaryOperator *inst,
                                 llvm::CallInst *callInst) {
  llvm::ConstantInt *v1, *v2;
  v1 = llvm::dyn_cast<llvm::ConstantInt>(inst->getOperand(0));
  v2 = llvm::dyn_cast<llvm::ConstantInt>(inst->getOperand(1));
  llvm::APInt res(256, 0, false);
  if (v1 && v2 && inst->getOpcode() == llvm::BinaryOperator::Add) {
    res = v1->getValue() + v2->getValue();
    llvm::Instruction *currentInst = inst;
    while ((llvm::Instruction *)currentInst != (llvm::Instruction *)callInst) {
      for (auto user = currentInst->user_begin();
           user != currentInst->user_end(); user++) {
        llvm::BinaryOperator *childUser =
            llvm::dyn_cast<llvm::BinaryOperator>(*user);
        if (!childUser) {
          continue;
        }
        v1 = llvm::dyn_cast<llvm::ConstantInt>(childUser->getOperand(0));
        v2 = llvm::dyn_cast<llvm::ConstantInt>(childUser->getOperand(1));
        if (v1 && !v2) {
          res = res + v1->getValue();
        } else if (!v1 && v2) {
          res = res + v2->getValue();
        } else {
          assert(false && "Arguments are not constant and a computed value. We "
                          "might be traversing a wrong folding chain");
        }
      }
      currentInst =
          llvm::dyn_cast<llvm::Instruction>(*currentInst->user_begin());
    }
  } else {
    assert(false && "Folding staring from wrong instruction");
  }
  return res.getSExtValue();
}

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
  retBuffer = visitor.CreateEntryBlockAlloca(
      visitor.currentFunction, "ret_buffer", visitor.getDefaultType());
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
  llvm::FunctionType *callFT = llvm::FunctionType::get(retType, argtyps, false);
  llvm::Function *callF = getOrCreateFunction("pyul_call_" + selector, callFT);
  if (retType->isPointerTy() &&
      retType->getPointerElementType()->isStructTy()) {
    llvm::StructType *retStructType =
        llvm::cast<llvm::StructType>(retType->getPointerElementType());
    llvm::CallInst *newCall =
        llvm::CallInst::Create(callF, args, "ret_struct", callInst);
    removeOldCallArgs(callInst);
    adjustCallReturns(callInst, newCall, retStructType, visitor);
  } else {
    llvm::CallInst *newCall = llvm::CallInst::Create(callF, args, "call_rv");
    removeOldCallArgs(callInst);
    llvm::ReplaceInstWithInst(callInst, newCall);
  }
}

void YulIntrinsicHelper::rewriteMapIndexCalls(llvm::CallInst *callInst) {
  llvm::SmallVector<llvm::Type *> argTypes;
  auto tmpBuilder = llvm::IRBuilder<>(callInst);
  llvm::Type *retType =
      llvm::Type::getIntNPtrTy(visitor.getContext(), 256, STORAGE_ADDR_SPACE);
  argTypes.push_back(retType);
  argTypes.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
  llvm::FunctionType *FT = llvm::FunctionType::get(retType, argTypes, false);
  llvm::Function *mapIndexFunction = getOrCreateFunction("pyul_map_index", FT);
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  if (offset) {
    auto varname = visitor.currentContract->getIdentifierDerefBySlotOffset(
        offset->getZExtValue(), 0);
    llvm::Value *ptrSelfVar = getPointerToStorageVarByName(varname, callInst);
    llvm::Type *mapType = checkAndGetPointeeType(ptrSelfVar);
    llvm::Value *mapPtr = tmpBuilder.CreateLoad(
        mapType, ptrSelfVar, getValueNameFromNamePath(varname));
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

void YulIntrinsicHelper::rewriteUpdateStorageByLocation(
    llvm::CallInst *callInst, llvm::Value *slot, llvm::Value *offset,
    llvm::Type *destType, llvm::Value *storeValue) {
  llvm::Value *castedValue = llvm::CastInst::CreateIntegerCast(
      storeValue, destType, true, "casted_" + storeValue->getName(), callInst);
  llvm::Align align = visitor.getModule().getDataLayout().getABITypeAlign(
      castedValue->getType());
  llvm::Value *storageLocation =
      getPointerInSlotByOffset(callInst, slot, offset);
  llvm::Value *castedStorageLocation = llvm::BitCastInst::Create(
      llvm::CastInst::CastOps::BitCast, storageLocation,
      destType->getPointerTo(STORAGE_ADDR_SPACE),
      "casted_" + storageLocation->getName(), callInst);
  auto newInst =
      new llvm::StoreInst(castedValue, castedStorageLocation, false, align);
  llvm::ReplaceInstWithInst(callInst, newInst);
}

void YulIntrinsicHelper::rewriteLoadStorageByLocation(llvm::CallInst *callInst,
                                                      llvm::Value *slot,
                                                      llvm::Value *offset,
                                                      llvm::Type *loadType) {
  llvm::Value *ptr = getPointerInSlotByOffset(callInst, slot, offset);
  llvm::Value *castedPtr =
      llvm::BitCastInst::Create(llvm::CastInst::CastOps::BitCast, ptr,
                                loadType->getPointerTo(STORAGE_ADDR_SPACE),
                                "casted_" + ptr->getName(), callInst);
  llvm::Align align =
      visitor.getModule().getDataLayout().getABITypeAlign(loadType);
  auto loadedValue = new llvm::LoadInst(
      loadType, castedPtr, "pyul_storage_var_load", false, align, callInst);
  llvm::Instruction *casted = llvm::CastInst::CreateIntegerCast(
      loadedValue, visitor.getDefaultType(), true);
  llvm::ReplaceInstWithInst(callInst, casted);
}

void YulIntrinsicHelper::rewriteLoadStorageVarByName(
    llvm::CallInst *callInst, std::vector<std::string> varName) {
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

void YulIntrinsicHelper::rewriteUpdateStorageVarByName(
    llvm::CallInst *callInst, std::vector<std::string> varname,
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
    llvm::CallInst *callInst, llvm::Value *slot, llvm::Value *offset) {
  llvm::IRBuilder<> tempBuilder(callInst);
  if (!slot->getType()->isPointerTy()) {
    slot = tempBuilder.CreateIntToPtr(slot,
                                      visitor.getDefaultType()->getPointerTo());
  }
  llvm::Constant *zeroConst = llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(visitor.getContext()), 0, 10);
  llvm::Value *offsetTrunc = tempBuilder.CreateIntCast(
      offset, llvm::Type::getInt32Ty(visitor.getContext()), false);
  llvm::ArrayType *slotArrayType =
      llvm::ArrayType::get(llvm::Type::getInt8Ty(visitor.getContext()), 32);
  llvm::Value *arrayCastedSlot = tempBuilder.CreatePointerCast(
      slot, slotArrayType->getPointerTo(STORAGE_ADDR_SPACE));
  llvm::Value *location =
      tempBuilder.CreateGEP(slotArrayType, arrayCastedSlot,
                            {zeroConst, offsetTrunc}, "ptr_slot_offset");
  return location;
}

void YulIntrinsicHelper::rewriteStorageDynamicLoadIntrinsic(
    llvm::CallInst *callInst, std::string yulTypeStr) {
  // R"(^read_from_storage_split_dynamic_(.*))"
  auto tempBuilder = llvm::IRBuilder<>(callInst);
  auto slot = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0));
  auto offset = llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(1));
  if (offset && slot) {
    std::vector<std::string> varname =
        visitor.currentContract->getIdentifierDerefBySlotOffset(
            slot->getZExtValue(), offset->getZExtValue());
    rewriteLoadStorageVarByName(callInst, varname);
  } else {
    rewriteLoadStorageByLocation(
        callInst, callInst->getArgOperand(0), callInst->getArgOperand(1),
        getTypeByTypeName(yulTypeStr, DEFAULT_ADDR_SPACE));
  }
}

void YulIntrinsicHelper::rewriteStorageOffsetLoadIntrinsic(
    llvm::CallInst *callInst, int offset, std::string yulTypeStr) {
  // Extract offset and load data type.
  int slot;
  if (llvm::ConstantInt *slotConst =
          llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(0))) {
    slot = slotConst->getZExtValue();
    std::vector<std::string> varname =
        visitor.currentContract->getIdentifierDerefBySlotOffset(slot, offset);
    rewriteLoadStorageVarByName(callInst, varname);
  } else {
    auto test = [](llvm::Instruction *inst) -> bool {
      llvm::BinaryOperator *addInst =
          llvm::dyn_cast<llvm::BinaryOperator>(inst);
      if (addInst && addInst->getOpcode() == llvm::BinaryOperator::Add) {
        llvm::Value *a1, *a2;
        a1 = llvm::dyn_cast<llvm::ConstantInt>(addInst->getOperand(0));
        a2 = llvm::dyn_cast<llvm::ConstantInt>(addInst->getOperand(1));
        if (a1 && a2) {
          return true;
        }
      }
      return false;
    };

    llvm::BinaryOperator *c =
        searchInstInDefs<llvm::BinaryOperator>(callInst, test);
    if (c) {
      int constSlot = foldAdds(c, callInst);
      std::vector<std::string> varname =
          visitor.currentContract->getIdentifierDerefBySlotOffset(constSlot,
                                                                  offset);
      rewriteLoadStorageVarByName(callInst, varname);
      return;
    }

    llvm::Type *loadType = getTypeByTypeName(yulTypeStr, DEFAULT_ADDR_SPACE);
    llvm::Constant *offsetConst =
        llvm::ConstantInt::get(visitor.getDefaultType(), offset, 10);
    rewriteLoadStorageByLocation(callInst, callInst->getArgOperand(0),
                                 offsetConst, loadType);
  }
}

void YulIntrinsicHelper::rewriteStorageLoadIntrinsic(llvm::CallInst *callInst) {
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  YUL_INTRINSIC_ID loadType = patternMatcher.getYulIntriniscType(calleeName);
  if (loadType == YUL_INTRINSIC_ID::READ_FROM_STORAGE_OFFSET) {
    auto res = patternMatcher.parseReadFromStorageOffset(calleeName);
    std::string typeStr =
        patternMatcher.readFromStorageOffsetGetType(calleeName);
    rewriteStorageOffsetLoadIntrinsic(callInst, res.offset, res.type);
  } else if (loadType == YUL_INTRINSIC_ID::READ_FROM_STORAGE_DYNAMIC) {
    auto res = patternMatcher.parseReadFromStorageDynamic(calleeName);
    rewriteStorageDynamicLoadIntrinsic(callInst, res.type);
  } else if (loadType == YUL_INTRINSIC_ID::READ_FROM_STORAGE_REFERENCE) {

  } else {
    //@todo raise runtime error
    assert(false && "unrecognized read from storage dynamic intrinsic");
  }
}

void YulIntrinsicHelper::rewriteStorageUpdateIntrinsic(
    llvm::CallInst *callInst) {
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  YUL_INTRINSIC_ID updateType = patternMatcher.getYulIntriniscType(calleeName);
  if (updateType == YUL_INTRINSIC_ID::UPDATE_STORAGE_OFFSET) {
    auto res = patternMatcher.parseUpdateStorageOffset(calleeName);
    rewriteStorageOffsetUpdateIntrinsic(callInst, res.offset, res.fromType,
                                        res.toType);
  } else if (updateType == YUL_INTRINSIC_ID::UPDATE_STORAGE_DYNAMIC) {
    auto res = patternMatcher.parseUpdateStorageDynamic(calleeName);
    rewriteStorageDynamicUpdateIntrinsic(callInst, res.fromType, res.toType);
  } else {
    // @todo raise runtime error
    assert(false && "update_storage_value did not match any regex");
  }
}

void YulIntrinsicHelper::rewriteStorageDynamicUpdateIntrinsic(
    llvm::CallInst *callInst, std::string srcTypeName,
    std::string destTypeName) {
  llvm::IRBuilder<> tempBuilder(callInst);
  assert(callInst->getNumArgOperands() == 4 &&
         "Wrong number of arguments to dynamic storage store inst");
  callInst->setName("");
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  llvm::Value *slotArg = callInst->getArgOperand(1);
  llvm::Value *offsetArg = callInst->getArgOperand(2);
  llvm::Value *storeValue = callInst->getArgOperand(3);
  auto slotConst = llvm::dyn_cast<llvm::ConstantInt>(slotArg);
  auto offsetConst = llvm::dyn_cast<llvm::ConstantInt>(offsetArg);
  if (slotConst && offsetConst) {
    std::vector<std::string> varname =
        visitor.currentContract->getIdentifierDerefBySlotOffset(
            slotConst->getZExtValue(), offsetConst->getZExtValue());
    rewriteUpdateStorageVarByName(callInst, varname, storeValue);
  } else {
    llvm::Type *destType = getTypeByTypeName(destTypeName, DEFAULT_ADDR_SPACE);
    llvm::Value *offsetIndex = tempBuilder.CreateIntCast(
        offsetArg, llvm::Type::getInt32Ty(visitor.getContext()), false,
        "array_offset");
    rewriteUpdateStorageByLocation(callInst, slotArg, offsetIndex, destType,
                                   storeValue);
  }
}

// truncate and write
void YulIntrinsicHelper::rewriteStorageOffsetUpdateIntrinsic(
    llvm::CallInst *callInst, int offset, std::string srcTypeName,
    std::string destTypeName) {
  llvm::IRBuilder<> tempBuilder(callInst);
  assert(callInst->getNumArgOperands() == 3 &&
         "Wrong number of arguments to storage store inst");
  callInst->setName("");
  std::string calleeName = callInst->getCalledFunction()->getName().str();
  llvm::Value *storeValue = callInst->getArgOperand(2);
  if (auto slot =
          llvm::dyn_cast<llvm::ConstantInt>(callInst->getArgOperand(1))) {
    std::vector<std::string> varname =
        visitor.currentContract->getIdentifierDerefBySlotOffset(
            slot->getZExtValue(), offset);
    rewriteUpdateStorageVarByName(callInst, varname, storeValue);
  } else {

    auto test = [](llvm::Instruction *inst) -> bool {
      llvm::BinaryOperator *addInst =
          llvm::dyn_cast<llvm::BinaryOperator>(inst);
      if (addInst && addInst->getOpcode() == llvm::BinaryOperator::Add) {
        llvm::Value *a1, *a2;
        a1 = llvm::dyn_cast<llvm::ConstantInt>(addInst->getOperand(0));
        a2 = llvm::dyn_cast<llvm::ConstantInt>(addInst->getOperand(1));
        if (a1 && a2) {
          return true;
        }
      }
      return false;
    };

    llvm::BinaryOperator *c =
        searchInstInDefs<llvm::BinaryOperator>(callInst, test);
    if (c) {
      int constSlot = foldAdds(c, callInst);
      std::vector<std::string> varname =
          visitor.currentContract->getIdentifierDerefBySlotOffset(constSlot,
                                                                  offset);
      rewriteUpdateStorageVarByName(callInst, varname, storeValue);
      return;
    }

    llvm::Type *destType = getTypeByTypeName(destTypeName, DEFAULT_ADDR_SPACE);
    llvm::Constant *offsetConst = llvm::ConstantInt::get(
        llvm::Type::getInt32Ty(visitor.getContext()), offset, 10);
    rewriteUpdateStorageByLocation(callInst, callInst->getArgOperand(1),
                                   offsetConst, destType, storeValue);
  }
}

void YulIntrinsicHelper::rewriteStorageArrayIndexAccess(
    llvm::CallInst *callInst) {
  assert(callInst->getNumArgOperands() == 3 &&
         "Wrong number of args in storage_array_index access");
  llvm::IRBuilder<> builder(callInst);
  std::regex storageArrayIndexRegex(
      R"(^storage_array_index_access_t_array\$_(.*)_\$(\d+)_storage$)");
  std::string name = callInst->getCalledFunction()->getName().str();
  std::smatch match;
  if (!std::regex_match(name, match, storageArrayIndexRegex)) {
    //@todo raise runtime error
    assert(false && "didnt match storage array index regex");
  }
  std::string elementTypeName = match[1].str();
  std::string sizeStr = match[2].str();
  int size;
  if (llvm::StringRef(sizeStr).getAsInteger(10, size)) {
    //@todo raise runtime error
    assert(false && "could not parse array size");
  }
  llvm::Type *elementType =
      getTypeByTypeName(elementTypeName, STORAGE_ADDR_SPACE);
  llvm::Value *slot = callInst->getArgOperand(1);
  llvm::Value *index = callInst->getArgOperand(2);
  assert(index->getType()->isIntegerTy() && "index is not int type");
  llvm::Value *truncIndex = builder.CreateIntCast(
      index, llvm::Type::getInt32Ty(visitor.getContext()), false, "index32");
  std::string indexString;
  auto offsetConst = llvm::dyn_cast<llvm::ConstantInt>(truncIndex);
  if (offsetConst) {
    indexString = std::to_string(offsetConst->getZExtValue());
  } else {
    indexString = truncIndex->getName();
  }
  auto slotConst = llvm::dyn_cast<llvm::ConstantInt>(slot);
  llvm::Value *arrayPtr;

  if (slotConst) {
    std::vector<std::string> arrayName =
        visitor.currentContract->getIdentifierDerefBySlotOffset(
            slotConst->getZExtValue(), 0);
    arrayPtr = getPointerToStorageVarByName(arrayName, callInst);
  } else {
    if (slot->getType()->isPointerTy())
      arrayPtr = builder.CreatePointerCast(
          slot, visitor.getDefaultType()->getPointerTo(STORAGE_ADDR_SPACE),
          slot->getName() + "_casted");
    else
      arrayPtr = builder.CreateIntToPtr(
          slot, visitor.getDefaultType()->getPointerTo(STORAGE_ADDR_SPACE),
          slot->getName() + "_casted");
  }
  llvm::Value *zero = llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(visitor.getContext()), 0, 10);
  arrayPtr = builder.CreateLoad(
      visitor.getDefaultType()->getPointerTo(STORAGE_ADDR_SPACE), arrayPtr,
      arrayPtr->getName().drop_front(4));
  llvm::Type *arrayType = llvm::ArrayType::get(elementType, 0);
  auto castedArrayPtr = builder.CreatePointerCast(
      arrayPtr, arrayType->getPointerTo(STORAGE_ADDR_SPACE));
  llvm::Value *elementPtr =
      builder.CreateGEP(arrayType, castedArrayPtr, {zero, truncIndex},
                        "ptr_" + arrayPtr->getName() + "[" + indexString + "]");
  // Now we have computed the pointer to return. But yul expectes two return
  // values. These values are packed in a struct. These will be unpacked in a
  // parent AST node that uses this struct
  llvm::Value *scalarizedElementPtr =
      builder.CreatePtrToInt(elementPtr, visitor.getDefaultType());
  llvm::Value *zero256 =
      llvm::ConstantInt::get(visitor.getDefaultType(), 0, 10);

  llvm::Value *structPtr =
      visitor.packRetsInStruct(name, {scalarizedElementPtr, zero256}, callInst);
  llvm::BasicBlock::InstListType &instList =
      callInst->getParent()->getInstList();
  llvm::BasicBlock::iterator callInstIt = callInst->getIterator();
  llvm::ReplaceInstWithValue(instList, callInstIt, structPtr);
}

void YulIntrinsicHelper::rewriteConvertStorageToStoragePtr(
    llvm::CallInst *callInst) {
  llvm::BasicBlock::InstListType &instList =
      callInst->getParent()->getInstList();
  llvm::BasicBlock::iterator callInstIt = callInst->getIterator();
  llvm::ReplaceInstWithValue(instList, callInstIt, callInst->getArgOperand(1));
}

void YulIntrinsicHelper::rewriteConvertStorageToMemoryPtr(
    llvm::CallInst *callInst, ConvertXToYResult res) {
  llvm::BasicBlock::InstListType &instList =
      callInst->getParent()->getInstList();
  llvm::BasicBlock::iterator callInstIt = callInst->getIterator();
  llvm::IRBuilder<> tmpBuilder(callInst);
  assert(res.sourceType == res.destType &&
         "Source and destination type are not same");
  YulStructTypeResult structType =
      patternMatcher.parseYulStructType(res.sourceType);
  auto sizeArray = visitor.getLLVMValueVector({structType.size});
  llvm::Value *newLocation = tmpBuilder.CreateCall(
      visitor.getAllocateMemoryFunction(), sizeArray, "new" + structType.name);
  auto destalign = visitor.getModule().getDataLayout().getABITypeAlign(
      newLocation->getType());
  llvm::Value *src = callInst->getArgOperand(1);
  llvm::Type *int8PtrType =
      llvm::Type::getInt8PtrTy(visitor.getContext(), STORAGE_ADDR_SPACE);
  if (!src->getType()->isPointerTy()) {
    src = tmpBuilder.CreateIntToPtr(src, int8PtrType, src->getName() + "_ptr");
  } else if (src->getType()->getIntegerBitWidth() != 8) {
    src = tmpBuilder.CreatePointerBitCastOrAddrSpaceCast(
        src, int8PtrType, src->getName() + "_ptr");
  }

  auto srcAlign =
      visitor.getModule().getDataLayout().getABITypeAlign(src->getType());
  tmpBuilder.CreateMemCpy(newLocation, destalign, src, srcAlign, sizeArray[0]);
  llvm::Value *castedNewLocation = tmpBuilder.CreatePtrToInt(
      newLocation, visitor.getDefaultType(), newLocation->getName() + "i256");
  llvm::ReplaceInstWithValue(instList, callInstIt, castedNewLocation);
}

void YulIntrinsicHelper::rewriteConvertXToY(llvm::CallInst *callInst) {
  ConvertXToYResult res =
      patternMatcher.parseConvertXToY(callInst->getCalledFunction()->getName());
  std::string structTypePrefix = "t_struct";
  if (res.sourceType.substr(0, structTypePrefix.size()) == structTypePrefix) {
    if (res.sourceAddressSpace == "storage" &&
        res.destAddressSpace == "storage")
      rewriteConvertStorageToStoragePtr(callInst);
    else if (res.sourceAddressSpace == "storage" &&
             res.destAddressSpace == "memory")
      rewriteConvertStorageToMemoryPtr(callInst, res);
  }
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
    } else if (c->getCalledFunction()->getName().startswith(
                   "storage_array_index_access_t_array")) {
      rewriteStorageArrayIndexAccess(c);
    } else if (c->getCalledFunction()->getName().startswith("convert_t_")) {
      rewriteConvertXToY(c);
    }
  }
}