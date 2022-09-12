#pragma once
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>

class YulIntrinsicEmitter{
    LLVMCodegenVisitor &visitor;
    public:
        bool isFunctionCallIntrinsic(std::string calleeName);
        llvm::Value* handleIntrinsicFunctionCall(YulFunctionCallNode &node);
        llvm::Value *handleAddFunctionCall(YulFunctionCallNode &node);
        llvm::Value *emitStorageLoadIntrinsic(YulFunctionCallNode &node);
        void emitStorageStoreIntrinsic(YulFunctionCallNode &node);
};