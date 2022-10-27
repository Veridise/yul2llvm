#pragma once
#include<llvm/IR/IRBuilder.h>
#include<libYulAST/YulASTVisitor/CodegenVisitor.h>
llvm::Value *getAddress(llvm::Value *);
std::string getSelector(llvm::Value *);
llvm::SmallVector<llvm::Value *> decodeArgsAndCleanup(llvm::Value *);
llvm::Value *getExtCallCtx(llvm::StringRef selector, llvm::Value *gas,
                           llvm::Value *address, llvm::Value *value,
                           llvm::Value *retBuffer, llvm::Value *retLen,
                           LLVMCodegenVisitor &v, llvm::IRBuilder<> &);

/**
 * @brief Adjust return values, remove the abidecode function call and store
 * the return values directly from call instruction
 * @todo handle array and mapping returns
 */
void adjustCallReturns(llvm::CallInst *, llvm::Value *returnedVals,
                       llvm::StructType *, LLVMCodegenVisitor &v);
llvm::Type *getExtCallReturnType(llvm::CallInst *callInst,
                                 LLVMCodegenVisitor &v, std::string name);
/**
 * @brief Remove the parameters encoded by abi_encode_xxx that are passed to
 * EVM call opcode. Arguments i.e. output of abi_encode_xxx
 * is the 5th argument (index 4) to call opcode.
 *
 * @param callInst
 */
void removeOldCallArgs(llvm::CallInst *callInst);