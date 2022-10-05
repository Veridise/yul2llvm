#include<libYulAST/YulASTVisitor/YulLLVMHelpers.h>

/**
 * @brief Remove the instruction and all the operands directly and transitively used.
 * The first instruction removed force removed and it is upon the caller to make sure that
 * it is safe to remove the instruction. 
 * 
 * @param i 
 */
void removeInstChains(llvm::Instruction *i){
    std::deque<llvm::Instruction*> toVisit;
    // collect all instructins that are directly or indirectly used in 
    // the inst to be removed. In bfs order. 
    toVisit.push_back(i);
    while (!toVisit.empty()) {
        llvm::Instruction *currentInst = toVisit.front();
        toVisit.pop_front();
        for (auto &op : currentInst->operands()) {
            if(auto inst = llvm::dyn_cast<llvm::Instruction>(op.get()))
                toVisit.push_back(inst);
        }
        if(currentInst == i || currentInst->getNumUses() == 0){
            currentInst->eraseFromParent();
        }
    }
}
    
