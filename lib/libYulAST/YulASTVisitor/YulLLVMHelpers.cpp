#include<libYulAST/YulASTVisitor/YulLLVMHelpers.h>

/**
 * @brief Remove the instruction and all the operands directly and transitively used.
 * The first instruction removed force removed and it is upon the caller to make sure that
 * it is safe to remove the instruction. 
 * 
 * @param i 
 */
void removeInstChains(llvm::Instruction *i){
    llvm::SmallVector<llvm::Instruction *> toVisit;
    llvm::SmallVector<llvm::Instruction *> potentiallyRemove;
    // collect all instructins that are directly or indirectly used in 
    // the inst to be removed. In bfs order. 
    toVisit.push_back(i);
    while (!toVisit.empty()) {
        llvm::Instruction *x = pop_front(toVisit);
        potentiallyRemove.push_back(i);
        for (auto &op : x->operands()) {
            llvm::Instruction *inst = llvm::dyn_cast<llvm::Instruction>(op.get());
            if (inst)
                toVisit.push_back(inst);
        }
    }
    potentiallyRemove[0]->eraseFromParent();
    potentiallyRemove.erase(potentiallyRemove.begin());
    for(auto &i: potentiallyRemove){
        if(i->getNumUses() == 0)
            i->eraseFromParent();
    }
    
}