#pragma once
class LLVMCodegenVisitor;
#include <libYulAST/YulASTVisitor/CodegenVisitor.h>

class YulFunctionDefinitionHelper{
    LLVMCodegenVisitor &visitor;
    // Intrinsics
    void createVarsForArgsAndRets(YulFunctionDefinitionNode &node, llvm::Function *F);
    public:
        llvm::Type *getReturnType(YulFunctionDefinitionNode &node);
        std::vector<llvm::Type *> getFunctionArgTypes(YulFunctionDefinitionNode &node);
        llvm::Function *createPrototype(YulFunctionDefinitionNode &node, llvm::SmallVector<llvm::Attribute::AttrKind> &attrs);
        std::unique_ptr<llvm::SmallVector<llvm::Attribute::AttrKind>> buildFunctionAttributes(YulFunctionDefinitionNode &node);
        void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &node);
        YulFunctionDefinitionHelper(LLVMCodegenVisitor &v);
};