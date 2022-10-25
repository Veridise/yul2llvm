#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/YulLLVMHelpers.h>
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
  if (auto zeroArgs = llvm::dyn_cast<llvm::ConstantInt>(encodedArgs)) {
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
    if (std::regex_match(callName, abiDecodeRegex)) {
      return true;
    } else {
      return false;
    }
  };
  llvm::CallInst *decodeCall = searchInstInUses<llvm::CallInst>(retBuff, test);
  return decodeCall;
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
  llvm::Type *retType;
  if (std::regex_match(decodeName, wholeMatch, abiDecodeRegex)) {
    std::string typesStr = wholeMatch[1].str();
    std::regex typesRegex(R"(((t_uint\d+)|(t_array.*)|(t_mapping.*)))");
    std::smatch typeMatch;
    while (std::regex_search(typesStr, typeMatch, typesRegex)) {
      if (typeMatch[2].matched) {
        std::string type = typeMatch[2].str();
        retType = v.getYulIntrisicHelper().getTypeByTypeName(type);
      } else if (typeMatch[3].matched || typeMatch[4].matched) {
        //@todo raise runtime error
        assert(false && "unhandled return types from external calls");
      }
      decodeRetTypes.push_back(retType);
      typesStr = typeMatch.suffix();
    }
  }
  llvm::StructType *rets, *retWithStatus;
  if (decodeRetTypes.size() == 1) {
    retWithStatus = llvm::StructType::create(
        v.getContext(), {v.getDefaultType(), v.getDefaultType()},
        selectorName + "_statusRetType");
    return retWithStatus->getPointerTo();
  } else if (decodeRetTypes.size() > 1) {
    rets = llvm::StructType::create(v.getContext(), decodeRetTypes,
                                    selectorName + "_retType");
    retWithStatus = llvm::StructType::create(
        v.getContext(), {v.getDefaultType(), rets->getPointerTo()},
        selectorName + "_statusRetType");
    return retWithStatus->getPointerTo();
  }
  return v.getDefaultType();
}

/**
 * @brief Adjust return values, remove the abidecode function call and store
 * the return values directly from call instruction
 * @todo handle array and mapping returns
 */
void adjustCallReturns(llvm::CallInst *callInst, llvm::Value *returnedVals,
                       llvm::StructType *retStructType, LLVMCodegenVisitor &v) {
  llvm::Instruction *decodeInstruction = findDecodeCall(callInst);
  llvm::BasicBlock::InstListType &decodeParentList =
      decodeInstruction->getParent()->getInstList();
  llvm::BasicBlock::iterator it = decodeInstruction->getIterator();
  llvm::Value *ptrVals = llvm::GetElementPtrInst::Create(
      retStructType, returnedVals, v.getLLVMValueVector({0, 1}), "ptr_returns",
      decodeInstruction);
  llvm::Value *vals = new llvm::LoadInst(
      ptrVals->getType()->getPointerElementType(), ptrVals, "returns", &*it++);
  it--;
  llvm::ReplaceInstWithValue(decodeParentList, it, vals);

  llvm::BasicBlock::InstListType &callInstParentList =
      callInst->getParent()->getInstList();
  it = callInst->getIterator();
  llvm::Value *ptrStatus = llvm::GetElementPtrInst::Create(
      retStructType, returnedVals, v.getLLVMValueVector({0, 0}), "ptr_status",
      callInst);
  llvm::Value *status =
      new llvm::LoadInst(ptrStatus->getType()->getPointerElementType(),
                         ptrStatus, "status", &*it++);
  it--;

  llvm::ReplaceInstWithValue(callInstParentList, it, status);
}

/**
 * @brief Remove the parameters encoded by abi_encode_xxx that are passed to
 * EVM call opcode. Arguments i.e. output of abi_encode_xxx
 * is the 5th argument (index 4) to call opcode.
 *
 * @param callInst
 */

void removeOldCallArgs(llvm::CallInst *callInst) {
  if (auto inst =
          llvm::dyn_cast<llvm::Instruction>(callInst->getArgOperand(4))) {
    if (inst->getNumUses() == 1) {
      removeInstChains(inst);
    }
  }
}