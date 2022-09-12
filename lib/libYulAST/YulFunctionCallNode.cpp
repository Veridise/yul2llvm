#include <cassert>
#include <iostream>
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulNodeBuilder.h>
#include <libYulAST/YulStringLiteralNode.h>

using namespace yulast;

void YulFunctionCallNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 1);
  callee = std::make_unique<YulIdentifierNode>(&topLevelChildren[0]);
  for (unsigned long i = 1; i < topLevelChildren.size(); i++) {
    args.push_back(YulExpressionBuilder::Builder(&topLevelChildren[i]));
  }
  if (!callee->getIdentfierValue().compare("revert")) {
    args.clear();
  }
}

YulFunctionCallNode::YulFunctionCallNode(const json *rawAST)
    : YulExpressionNode(
          rawAST,
          YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_FUNCTION_CALL) {
  assert(sanityCheckPassed(rawAST, YUL_FUNCTION_CALL_KEY));
  parseRawAST(rawAST);
}

std::string YulFunctionCallNode::to_string() {
  if (!str.compare("")) {
    str.append(callee->to_string());
    str.append("(");
    for (auto &arg : args) {
      str.append(arg->to_string()).append(",");
    }
    str.append(")");
  }
  return str;
}



llvm::Type *YulFunctionCallNode::getReturnType() {
  if (!callee->getIdentfierValue().compare("revert"))
    return llvm::Type::getVoidTy(*TheContext);
  return llvm::Type::getIntNTy(*TheContext, 256);
}

void YulFunctionCallNode::createPrototype() {
  std::vector<llvm::Type *> funcArgTypes = getFunctionArgs();
  llvm::Type *retType = getReturnType();
  FT = llvm::FunctionType::get(retType, funcArgTypes, false);

  F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             callee->getIdentfierValue(), TheModule.get());

  if (!callee->getIdentfierValue().compare("revert")) {
    F->addAttribute(0, llvm::Attribute::NoReturn);
  }
}





llvm::Value *YulFunctionCallNode::codegen(llvm::Function *enclosingFunction) {
  if (callee->getIdentfierValue() == "pyul_storage_var_load") {
    return emitStorageLoadIntrinsic(enclosingFunction);
  } else if (callee->getIdentfierValue() == "pyul_storage_var_update") {
    return emitStorageStoreIntrinsic(enclosingFunction);
  }
  if (!F)
    F = TheModule->getFunction(callee->getIdentfierValue());

  if (!F)
    createPrototype();

  assert(F && "Function not found and could not be created");

  if (!callee->getIdentfierValue().compare("checked_add_t_uint256")) {
    llvm::Value *v1, *v2;
    v1 = args[0]->codegen(enclosingFunction);
    v2 = args[1]->codegen(enclosingFunction);
    return Builder->CreateAdd(v1, v2);
  }
  std::vector<llvm::Value *> ArgsV;

  for (auto &a : args) {
    llvm::Value *lv = a->codegen(enclosingFunction);
    ArgsV.push_back(lv);
  }
  // std::cout<<"Creating call "<<callee->getIdentfierValue()<<std::endl;
  if (F->getReturnType() == llvm::Type::getVoidTy(*TheContext)) {
    Builder->CreateCall(F, ArgsV);
    return nullptr;
  } else {
    return Builder->CreateCall(F, ArgsV, callee->getIdentfierValue());
  }
}

std::string YulFunctionCallNode::getCalleeName() {
  return callee->getIdentfierValue();
}

std::vector<std::unique_ptr<YulExpressionNode>> &
YulFunctionCallNode::getArgs() {
  return args;
}