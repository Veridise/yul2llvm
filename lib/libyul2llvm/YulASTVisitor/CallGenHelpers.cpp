#include "CallGenHelpers.h"
#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>
#include <libyul2llvm/YulASTVisitor/YulLLVMHelpers.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/FormatVariadic.h>
#include <regex>

llvm::Value *getAddress(llvm::Value *address) {
  llvm::CallInst *mayBeContractToAddr = llvm::dyn_cast<llvm::CallInst>(address);
  assert(mayBeContractToAddr && "Address is not a call instruction");
  if (mayBeContractToAddr) {
    assert(mayBeContractToAddr->getNumArgOperands() == 2 &&
           "Incorrect number of arguments in address for call inst");
    std::regex castContToAddrRE(
        "convert_t_(contract|address)(.*)_to_t_address");
    std::regex castAddrToContRE(
        "convert_t_address(_payable)?_to_t_(contract|address).*");
    if (std::regex_match(
            mayBeContractToAddr->getCalledFunction()->getName().str(),
            castContToAddrRE)) {
      llvm::CallInst *mayBeAddrToContract =
          llvm::dyn_cast<llvm::CallInst>(mayBeContractToAddr->getArgOperand(1));
      assert(mayBeAddrToContract && "casting issues in external call");
      if (std::regex_match(
              mayBeAddrToContract->getCalledFunction()->getName().str(),
              castAddrToContRE)) {
        llvm::Value *addr = mayBeAddrToContract->getArgOperand(1);
        mayBeAddrToContract->eraseFromParent();
        mayBeContractToAddr->eraseFromParent();
        return addr;
      }
    }
  }
  assert(false && "Could not infer callee address");
  return (llvm::Value *)nullptr;
}

std::string getSelector(llvm::Value *selector) {
  llvm::ConstantInt *selectorNumber =
      llvm::dyn_cast<llvm::ConstantInt>(selector);
  assert(selectorNumber && "selector is not constant");
  return llvm::formatv("{0:x}", selectorNumber->getZExtValue());
}

llvm::SmallVector<llvm::Value *>
decodeArgsAndCleanup(llvm::Value *encodedArgs) {
  llvm::SmallVector<llvm::Value *> args;
  llvm::CallInst *abiEncodeCall = nullptr;
  if (llvm::dyn_cast<llvm::ConstantInt>(encodedArgs)) {
    return args;
  }
  auto sub = llvm::dyn_cast<llvm::BinaryOperator>(encodedArgs);
  assert(sub && "Encoded args not sub");

  const auto test = [](llvm::Instruction *inst) -> bool {
    auto callInst = llvm::dyn_cast<llvm::CallInst>(inst);
    if (!callInst)
      return false;
    auto name = callInst->getName();
    std::string encoderFunctionPrefix = "abi_encode_";
    return name.startswith(encoderFunctionPrefix);
  };
  if (sub) {
    abiEncodeCall = searchInstInDefs<llvm::CallInst>(sub, test);
    if (abiEncodeCall) {
      int numArgs = abiEncodeCall->getNumArgOperands();
      for (int i = 1; i < numArgs; i++)
        args.push_back(abiEncodeCall->getArgOperand(i));
    } else {
      assert(false && "Did not find abi_encode funciton");
    }
  }
  return args;
}

llvm::Value *getExtCallCtx(llvm::StringRef selector, llvm::Value *gas,
                           llvm::Value *address, llvm::Value *value,
                           llvm::Value *retBuffer, llvm::Value *retLen,
                           LLVMCodegenVisitor &v, llvm::IRBuilder<> &builder) {
  llvm::Value *elementPtr;
  llvm::AllocaInst *ptrExtCallCtx = v.CreateEntryBlockAlloca(
      v.currentFunction, selector.str() + "ctx", v.getExtCallCtxType());
  llvm::SmallVector<llvm::Value *> indices;
  // gas
  indices = v.getLLVMValueVector({0, 0});
  elementPtr = builder.CreateGEP(v.getExtCallCtxType(), ptrExtCallCtx, indices,
                                 "gasPtr");
  builder.CreateStore(gas, elementPtr);

  // address
  indices = v.getLLVMValueVector({0, 1});
  elementPtr = builder.CreateGEP(v.getExtCallCtxType(), ptrExtCallCtx, indices,
                                 "addrPtr");
  builder.CreateStore(address, elementPtr);

  // value
  indices = v.getLLVMValueVector({0, 2});
  elementPtr = builder.CreateGEP(v.getExtCallCtxType(), ptrExtCallCtx, indices,
                                 "valuePtr");
  builder.CreateStore(value, elementPtr);

  // buffer
  indices = v.getLLVMValueVector({0, 3});
  elementPtr = builder.CreateGEP(v.getExtCallCtxType(), ptrExtCallCtx, indices,
                                 "bufferPtr");
  builder.CreateStore(retBuffer, elementPtr);

  // retLen
  indices = v.getLLVMValueVector({0, 4});
  elementPtr = builder.CreateGEP(v.getExtCallCtxType(), ptrExtCallCtx, indices,
                                 "retLenPtr");
  builder.CreateStore(retLen, elementPtr);
  return ptrExtCallCtx;
}

llvm::CallInst *findDecodeCall(llvm::CallInst *callInst) {
  llvm::Value *retBuff = callInst->getArgOperand(5);
  std::regex abiDecodeRegex(R"(abi_decode_(.*)*_from.*)");
  std::smatch wholeMatch;
  const auto test = [abiDecodeRegex](llvm::CallInst *call) -> bool {
    std::string callName = call->getCalledFunction()->getName().str();
    return std::regex_match(callName, abiDecodeRegex);
  };
  return searchInstInUses<llvm::CallInst>(retBuff, test);
}

llvm::Type *getExtCallReturnType(llvm::CallInst *callInst,
                                 LLVMCodegenVisitor &v,
                                 std::string selectorName) {
  llvm::CallInst *decodeCall = findDecodeCall(callInst);
  if (!decodeCall) {
    return v.getDefaultType();
  }
  llvm::SmallVector<llvm::Type *> decodeRetTypes;
  std::string decodeName = decodeCall->getCalledFunction()->getName().str();
  std::regex abiDecodeRegex(R"(abi_decode_tuple_(.*)_from.*)");
  std::smatch wholeMatch;
  if (std::regex_match(decodeName, wholeMatch, abiDecodeRegex)) {
    std::string typesStr = wholeMatch[1].str();
    std::regex typesRegex(R"(((t_uint\d+)|(t_array.*)|(t_mapping.*)))");
    std::smatch typeMatch;
    llvm::Type *retType;
    while (std::regex_search(typesStr, typeMatch, typesRegex)) {
      if (typeMatch[2].matched) {
        std::string type = typeMatch[2].str();
        retType = v.getYulIntrisicHelper().getTypeByTypeName(type, DEFAULT_ADDR_SPACE);
      } else if (typeMatch[3].matched || typeMatch[4].matched) {
        //@todo raise runtime error
        assert(false && "unhandled return types from external calls");
      }
      decodeRetTypes.push_back(retType);
      typesStr = typeMatch.suffix();
    }
  }
  llvm::StructType *retWithStatus;
  if (decodeRetTypes.size() == 0) {
    // The external call has a void return type, therefore just return the
    // status
    return v.getDefaultType();
  } else if (decodeRetTypes.size() == 1) {
    // return struct {status of the call, return value}
    retWithStatus = llvm::StructType::create(
        v.getContext(), {v.getDefaultType(), v.getDefaultType()},
        selectorName + "_statusRetType");
    return retWithStatus->getPointerTo();
  } else if (decodeRetTypes.size() > 1) {
    llvm::StructType *rets;
    // return struct {status of the call, struct packing multiple return values}
    rets = llvm::StructType::create(v.getContext(), decodeRetTypes,
                                    selectorName + "_retType");
    retWithStatus = llvm::StructType::create(
        v.getContext(), {v.getDefaultType(), rets->getPointerTo()},
        selectorName + "_statusRetType");
    return retWithStatus->getPointerTo();
  }
  //@todo raise runtime error
  assert(false && "Unhandled case in getExtCallRetTypes");
  return nullptr;
}

void adjustCallReturns(llvm::CallInst *callInst, llvm::Value *returnedVals,
                       llvm::StructType *retStructType, LLVMCodegenVisitor &v) {
  llvm::Instruction *decodeInstruction = findDecodeCall(callInst);
  llvm::BasicBlock::InstListType &decodeParentList =
      decodeInstruction->getParent()->getInstList();
  llvm::BasicBlock::iterator it = decodeInstruction->getIterator();
  llvm::Value *ptrVals = llvm::GetElementPtrInst::Create(
      retStructType, returnedVals, v.getLLVMValueVector({0, 1}), "ptr_returns",
      decodeInstruction);
  // We cannot insert instruction that is replacing before the instruction we
  // are replacing so we need to find the next instruction.
  llvm::Instruction &deocodeNextInstRef = *(it++);
  // reset the iterator to decode inst
  it--;
  llvm::Value *vals =
      new llvm::LoadInst(ptrVals->getType()->getPointerElementType(), ptrVals,
                         "returns", &deocodeNextInstRef);
  llvm::ReplaceInstWithValue(decodeParentList, it, vals);

  llvm::BasicBlock::InstListType &callInstParentList =
      callInst->getParent()->getInstList();
  it = callInst->getIterator();
  llvm::Value *ptrStatus = llvm::GetElementPtrInst::Create(
      retStructType, returnedVals, v.getLLVMValueVector({0, 0}), "ptr_status",
      callInst);
  llvm::Instruction &callNextInstRef = *(it++);
  it--;
  llvm::Value *status =
      new llvm::LoadInst(ptrStatus->getType()->getPointerElementType(),
                         ptrStatus, "status", &callNextInstRef);

  llvm::ReplaceInstWithValue(callInstParentList, it, status);
}

void removeOldCallArgs(llvm::CallInst *callInst) {
  if (auto inst =
          llvm::dyn_cast<llvm::Instruction>(callInst->getArgOperand(4))) {
    if (inst->getNumUses() == 1) {
      removeInstChains(inst);
    }
  }
}