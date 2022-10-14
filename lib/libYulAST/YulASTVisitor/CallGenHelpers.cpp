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
    if (std::regex_match(mayBeContractToAddr->getName().str(),
                         castContToAddrRE)) {
      llvm::CallInst *mayBeAddrToContract =
          llvm::dyn_cast<llvm::CallInst>(mayBeContractToAddr->getArgOperand(1));
      assert(mayBeAddrToContract && "casting issues in external call");
      if (std::regex_match(mayBeAddrToContract->getName().str(),
                           castAddrToContRE)) {
      llvm::outs()<<"HERE\n";
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
  if(auto zeroArgs = llvm::dyn_cast<llvm::ConstantInt>(encodedArgs)){
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
    if (inst->getNumUses() == 1)
      removeInstChains(inst);
  }
}