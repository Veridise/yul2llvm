#pragma once
#include <llvm/IR/Function.h>

template<class SearchInstType, typename Test>
SearchInstType *searchInstInDefs(llvm::Instruction *,Test t);


template<class SearchInstType, typename Test>
SearchInstType *searchInstInDefs(llvm::Instruction *, Test);
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
      //@technius This seems like a redudant cast, as the lambda is also casting
      // is this okay or can we micro optimize this further?

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

template<typename ElemType> 
ElemType pop_front(llvm::SmallVector<ElemType> &v){
    ElemType ret = v[0];
    v.erase(v.begin()); 
    return ret;
}
void removeInstChains(llvm::Instruction *);
