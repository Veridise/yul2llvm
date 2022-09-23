#include <llvm/IR/Value.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <regex>
#include <llvm/Support/FormatVariadic.h>

llvm::Value *getAddress(llvm::Value *address){
    llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>(address);
    assert(callInst && "Address is not a call instruction");
    if(callInst){
        assert(callInst->getNumArgOperands() == 1 && "Incorrect number of arguments in address for call inst");
        std::regex castContToAddrRE("convert_t_contract\\$_(.*)_\\$(\\d+)_to_t_address");
        std::regex castAddrToContRE("convert_t_address(_payable)?_to_t_contract\\$_(.*)_\\$(\\d*)");
        if(std::regex_match(callInst->getName().str(), castContToAddrRE)){
            callInst = llvm::dyn_cast<llvm::CallInst>(callInst->getArgOperand(0));
            assert(callInst && "casting issues in external call");
            if(std::regex_match(callInst->getName().str(), castAddrToContRE)){
                return callInst->getArgOperand(0);
            }
            
        }
    }
    assert(false && "Could not infer callee address");
    return (llvm::Value*)nullptr;
}

llvm::Value* getSelector(llvm::Value *selector, LLVMCodegenVisitor &v){
    return nullptr;
}

llvm::Value *getCallArgs(llvm::Value *encodedArgs){
    return (llvm::Value*)0;
}