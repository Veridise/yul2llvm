#pragma once
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicEmitter.h>

class YulFunctionCallHelper{
    LLVMCodegenVisitor &visitor;
    // Intrinsics
    YulIntrinsicEmitter intrinsicEmitter;
    public:
        llvm::Type *getReturnType(YulFunctionCallNode &node);
        std::vector<llvm::Type *> getFunctionArgTypes(YulFunctionCallNode &node);
        llvm::Function *createPrototype(YulFunctionCallNode &node, llvm::SmallVector<llvm::Attribute::AttrKind> &attrs);
        std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>> buildFunctionAttributes(YulFunctionCallNode &node);
        llvm::Value *visitYulFunctionCallNode(YulFunctionCallNode &node);
        YulFunctionCallHelper(LLVMCodegenVisitor &visitor);
        
};