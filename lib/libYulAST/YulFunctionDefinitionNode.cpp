#include <cassert>
#include <fstream>
#include <iostream>
#include <libYulAST/YulFunctionDefinitionNode.h>

using namespace yulast;

void YulFunctionDefinitionNode::parseRawAST() {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 2);
  for (json::iterator it = topLevelChildren.begin();
       it != topLevelChildren.end(); it++) {
    if (!(*it)["type"].get<std::string>().compare(YUL_IDENTIFIER_KEY))
      functionName = new YulIdentifierNode(&(*it));
    else if (!(*it)["type"].get<std::string>().compare(
                 YUL_FUNCTION_ARG_LIST_KEY))
      args = new YulFunctionArgListNode(&(*it));
    else if (!(*it)["type"].get<std::string>().compare(
                 YUL_FUNCTION_RET_LIST_KEY))
      rets = new YulFunctionRetListNode(&(*it));
    else if (!(*it)["type"].get<std::string>().compare(YUL_BLOCK_KEY))
      body = new YulBlockNode(&(*it));
  }
}

YulFunctionDefinitionNode::YulFunctionDefinitionNode(json *rawAST)
    : YulStatementNode(rawAST, YUL_AST_STATEMENT_FUNCTION_DEFINITION) {
  assert(sanityCheckPassed(YUL_FUNCTION_DEFINITION_KEY));
  parseRawAST();
}

std::string YulFunctionDefinitionNode::to_string() {
  if (!str.compare("")) {
    str.append("define ");
    str.append(functionName->to_string());
    str.append("(");
    if(args != NULL)
      str.append(args->to_string());
    str.append(")");
    if(rets != NULL)
      str.append(rets->to_string());
    str.append(body->to_string());
    str.append("}");
  }
  return str;
}

void YulFunctionDefinitionNode::createPrototype() {
  int numargs;
  if (args == NULL)
    numargs = 0;
  else
    numargs = args->getIdentifiers().size();

  std::vector<llvm::Type *> funcArgTypes(numargs,
                                         llvm::Type::getInt32Ty(*TheContext));

  FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext),
                               funcArgTypes, false);

  F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             functionName->getIdentfierValue(),
                             TheModule.get());

  int idx = 0;
  for (auto &arg : F->args()) {
    arg.setName(args->getIdentifiers().at(idx++)->getIdentfierValue());
  }
}

void YulFunctionDefinitionNode::createVarsForArgsAndRets() {
  llvm::BasicBlock *BB =
      llvm::BasicBlock::Create(*(YulASTBase::TheContext), "entry", F);
  Builder->SetInsertPoint(BB);



  if (args != NULL) {
    for (auto arg : args->getIdentifiers()) {
      llvm::AllocaInst *a = CreateEntryBlockAlloca(F, arg->getIdentfierValue().append("_arg"));
      NamedValues[arg->getIdentfierValue()] = a;
    }
  }

  for(auto &f: F->args()){
    Builder->CreateStore(&f, NamedValues[f.getName().str()]);
  }

  if (rets != NULL) {
    for (auto arg : rets->getIdentifiers()) {
      llvm::AllocaInst *a = CreateEntryBlockAlloca(F, arg->getIdentfierValue());
      NamedValues[arg->getIdentfierValue()] = a;
    }
  }
}

llvm::Value *
YulFunctionDefinitionNode::codegen(llvm::Function *placeholderFunc) {
  if (!F)
    createPrototype();
  createVarsForArgsAndRets();
  body->codegen(F);
  if(!rets){
    Builder->CreateRetVoid();
  }
  else {
    // TODO assuming rets has only a single element
    llvm::Value *v = Builder->CreateLoad(
        llvm::Type::getInt32Ty(*TheContext),
        NamedValues[rets->getIdentifiers()[0]->getIdentfierValue()]);
    Builder->CreateRet(v);
  }
  llvm::verifyFunction(*F, &(llvm::errs()));

  return nullptr;
}

void YulFunctionDefinitionNode::dumpToFile(std::string outputFileName) {
  std::error_code fileOpeningError;
  llvm::raw_fd_ostream f(outputFileName, fileOpeningError);
  TheModule->print(f, nullptr);
}

void YulFunctionDefinitionNode::dumpToStdout() {
  TheModule->print(llvm::errs(), nullptr);
}
