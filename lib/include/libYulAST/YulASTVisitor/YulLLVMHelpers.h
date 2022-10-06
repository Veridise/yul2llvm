#pragma once
#include <llvm/IR/Function.h>
#include <deque>
/**
 * @brief Search for passing certain test(expressed via lambda expr)
 *   in use def chain of i
 * 
 * @tparam SearchInstType Type of instruction to search
 * @tparam Test: labda expression to represent search criteria
 * @param i The instruction looking for search function
 * @param test 
 * @return SearchInstType* 
 */
template<class SearchInstType, typename Test>
SearchInstType *searchInstInDefs(llvm::Instruction *i, Test test){
    llvm::SmallVector<llvm::Instruction *> toVisit;
    toVisit.push_back(i);
    while (!toVisit.empty()) {
      llvm::Instruction *x = toVisit.pop_back_val();
      auto currentInst = llvm::dyn_cast<SearchInstType>(x);
      if (currentInst && test(currentInst)) {
        return currentInst;
      }
      for (auto &op : x->operands()) {
        llvm::Instruction *inst = llvm::dyn_cast<llvm::Instruction>(op.get());
        if (inst)
          toVisit.push_back(inst);
      }
    }
    return nullptr;
}

/**
 * @brief Remove the instruction and all the operands directly and transitively used.
 * The first instruction removed force removed and it is upon the caller to make sure that
 * it is safe to remove the instruction. 
 * 
 * @param i 
 */
void removeInstChains(llvm::Instruction *);
