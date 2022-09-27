#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/FormatVariadic.h>
#include <regex>

llvm::Value *getAddress(llvm::Value *address) {
  llvm::CallInst *mayBeContToAddr = llvm::dyn_cast<llvm::CallInst>(address);
  assert(mayBeContToAddr && "Address is not a call instruction");
  if (mayBeContToAddr) {
    assert(mayBeContToAddr->getNumArgOperands() == 1 &&
           "Incorrect number of arguments in address for call inst");
    std::regex castContToAddrRE(
        "convert_t_contract\\$_(.*)_\\$(\\d+)_to_t_address");
    std::regex castAddrToContRE(
        "convert_t_address(_payable)?_to_t_contract\\$_(.*)_\\$(\\d*)");
    if (std::regex_match(mayBeContToAddr->getName().str(), castContToAddrRE)) {
      llvm::CallInst *mayBeAddrToCont =
          llvm::dyn_cast<llvm::CallInst>(mayBeContToAddr->getArgOperand(0));
      assert(mayBeAddrToCont && "casting issues in external call");
      if (std::regex_match(mayBeAddrToCont->getName().str(),
                           castAddrToContRE)) {
        llvm::Value *addr = mayBeAddrToCont->getArgOperand(0);
        mayBeAddrToCont->eraseFromParent();
        mayBeContToAddr->eraseFromParent();
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

llvm::SmallVector<llvm::Value *> getCallArgs(llvm::Value *encodedArgs) {
  llvm::SmallVector<llvm::Value *> args;
  llvm::CallInst *abiEncodeCall = nullptr;
  llvm::SmallVector<llvm::Instruction *> toVisit;
  llvm::SmallVector<llvm::Instruction *> toRemove;
  auto sub = llvm::dyn_cast<llvm::BinaryOperator>(encodedArgs);
  assert(sub && "Encoded args not sub");
  if (sub) {
    toVisit.push_back(sub);
    while (!toVisit.empty()) {
      llvm::Instruction *x = toVisit.pop_back_val();
      toRemove.push_back(x);
      auto callInst = llvm::dyn_cast<llvm::CallInst>(x);
      if (callInst) {
        auto name = callInst->getName();
        std::string encoderFunctionPrefix = "abi_encode_";
        if (name.startswith(encoderFunctionPrefix)) {
          abiEncodeCall = callInst;
          break;
        }
      }
      for (auto &op : x->operands()) {
        llvm::Instruction *inst = llvm::dyn_cast<llvm::Instruction>(op.get());
        if (inst)
          toVisit.push_back(inst);
      }
    }

    assert(abiEncodeCall && "abiEncodeCall is not a call instruction");
    if (abiEncodeCall) {
      int numArgs = abiEncodeCall->getNumArgOperands();
      for (int i = 1; i < numArgs; i++)
        args.push_back(abiEncodeCall->getArgOperand(i));
      for (auto &u : abiEncodeCall->operands()) {
        llvm::Instruction *inst = llvm::dyn_cast<llvm::Instruction>(u.get());
        llvm::Value *val = llvm::dyn_cast<llvm::Value>(u.get());
        if (inst) {
          auto it = std::find(args.begin(), args.end(), val);
          if (it == args.end())
            toRemove.push_back(inst);
        }
      }

      if (toRemove[0]->getNumUses() == 1) {
        toRemove[0]->eraseFromParent();
        toRemove.erase(toRemove.begin());
        for (auto i : toRemove) {
          if (i->getNumUses() == 0) {
            i->eraseFromParent();
          }
        }
      }
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