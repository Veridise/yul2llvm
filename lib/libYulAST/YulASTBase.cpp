#include <iostream>
#include <libYulAST/YulASTBase.h>

using namespace yulast;
std::unique_ptr<llvm::LLVMContext> YulASTBase::TheContext =
    std::make_unique<llvm::LLVMContext>();
std::unique_ptr<llvm::Module> YulASTBase::TheModule =
    std::make_unique<llvm::Module>("yul", *TheContext);
std::unique_ptr<llvm::IRBuilder<>> YulASTBase::Builder =
    std::make_unique<llvm::IRBuilder<>>(*TheContext);
std::map<std::string, llvm::AllocaInst *> YulASTBase::NamedValues;
std::map<std::string, std::string> YulASTBase::literalNames; 


bool YulASTBase::sanityCheckPassed(const json *rawAST, std::string key) {
  if (!rawAST->contains("type")) {
    std::cout << "type not present" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  if (!rawAST->contains("children")) {
    std::cout << "children not present" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  if ((rawAST->size() != 2)) {
    std::cout << "size not equal 2" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  if ((*rawAST)["type"].get<std::string>().compare(key)) {
    std::cout << "wrong key" << std::endl;
    std::cout << rawAST->dump() << std::endl;
    return false;
  }
  return true;
}

std::string YulASTBase::to_string() { return "to_str not defined for base"; }

void YulASTBase::parseRawAST(const json *rawAST) {
  std::cout << "Parsing Not Implemented" << std::endl;
}

llvm::Value *YulASTBase::codegen(llvm::Function *F) {
  std::cout << "Codegen not implemented" << std::endl;
  return nullptr;
}

YulASTBase::YulASTBase(const json *rawAST, YUL_AST_NODE_TYPE nodeType)
    : nodeType(nodeType) {}

llvm::AllocaInst *
YulASTBase::CreateEntryBlockAlloca(llvm::Function *TheFunction,
                                   const std::string &VarName) {
  llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                         TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(llvm::Type::getIntNTy(*TheContext, 256), 0,
                           VarName.c_str());
}
