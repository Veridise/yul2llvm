#include <libYulAST/YulASTVisitor/VisitorBase.h>

class LLVMCodegenVisitor : public VisitorBase {
    public:
        virtual void visitYulAssignmentNode(YulAssignmentNode &) override;
        virtual void visitYulBlockNode(YulBlockNode &) override;
        virtual void visitYulBreakNode(YulBreakNode &) override;
        virtual void visitYulCaseNode(YulCaseNode &) override;
        virtual void visitYulContinueNode(YulContinueNode &) override;
        virtual void visitYulContractNode(YulContractNode &) override;
        virtual void visitYulDefaultNode(YulDefaultNode &) override;
        virtual void visitYulForNode(YulForNode &) override;
        virtual llvm::Value *visitYulFunctionCallNode(YulFunctionCallNode &) override;
        virtual void visitYulFunctionDefinitionNode(YulFunctionDefinitionNode &) override;
        virtual llvm::Value *visitYulIdentifierNode(YulIdentifierNode &) override;
        virtual void visitYulIfNode(YulIfNode &) override;
        virtual void visitYulLeaveNode(YulLeaveNode &) override;
        virtual llvm::Value *visitYulNumberLiteralNode(YulNumberLiteralNode &) override;
        virtual llvm::Value *visitYulStringLiteralNode(YulStringLiteralNode &) override;
        virtual void visitYulSwitchNode(YulSwitchNode &) override;
        virtual void visitYulVariableDeclarationNode(YulVariableDeclarationNode &) override;
        virtual ~LLVMCodegenVisitor(){}

        LLVMCodegenVisitor();

        //LLVM datastructures
        llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                                        const std::string &VarName);
        llvm::GlobalVariable *
        CreateGlobalStringLiteral(std::string literalValue, std::string literalName);
        std::unique_ptr<llvm::LLVMContext> TheContext;
        std::unique_ptr<llvm::Module> TheModule;
        std::unique_ptr<llvm::IRBuilder<>> Builder;
        llvm::StringMap<llvm::AllocaInst *> NamedValues;
        llvm::StringMap<std::string> stringLiteralNames;
        // data structures for self
        llvm::SmallVector<std::string> structFieldOrder;
        llvm::StringMap<std::tuple<std::string, int>> typeMap;
        llvm::StructType *selfType;
        llvm::GlobalVariable *self;
        llvm::Module &getModule();
        llvm::IRBuilder<> &getBuilder();
        llvm::LLVMContext &getContext();
        llvm::StringMap<llvm::AllocaInst *> &getNamedValuesMap();

        llvm::Value *visit(YulASTBase &);
        llvm::Value *visitYulExpressionNode(YulExpressionNode &);
        void visitYulStatementNode(YulStatementNode &);
        llvm::Value *visitYulLiteralNode(YulLiteralNode &);
        std::stack<std::tuple<llvm::BasicBlock*, llvm::BasicBlock*>> loopControlFlowBlocks;
};