#include <cassert>
#include <iostream>
#include <libYulAST/YulFunctionCallNode.h>
#include <libYulAST/YulNodeBuilder.h>

using namespace yulast;

void YulFunctionCallNode::parseRawAST(const json *rawAST) {
  json topLevelChildren = rawAST->at("children");
  assert(topLevelChildren.size() >= 1);
  callee = std::make_unique<YulIdentifierNode>(&topLevelChildren[0]);
  for (unsigned long i = 1; i < topLevelChildren.size(); i++) {
    args.push_back(YulExpressionBuilder::Builder(&topLevelChildren[i]));
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

void YulFunctionCallNode::createPrototype() {
  int numargs;
  if (args.size() == 0)
    numargs = 0;
  else
    numargs = getArgs().size();

  std::vector<llvm::Type *> funcArgTypes(numargs,
                                         llvm::Type::getInt32Ty(*TheContext));

  FT = llvm::FunctionType::get(llvm::Type::getInt32Ty(*TheContext),
                               funcArgTypes, false);

  F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             callee->getIdentfierValue(), TheModule.get());
}

llvm::Value *YulFunctionCallNode::codegen(llvm::Function *enclosingFunction) {
  if (!F)
    F = TheModule->getFunction(callee->getIdentfierValue());

  if (!F)
    createPrototype();
  if (!F) {
    std::cout << "Function not found and could not be created" << std::endl;
    exit(1);
  }
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
  return Builder->CreateCall(F, ArgsV, callee->getIdentfierValue());
}

std::string YulFunctionCallNode::getName() {
  return callee->getIdentfierValue();
}

std::vector<std::unique_ptr<YulExpressionNode>> &
YulFunctionCallNode::getArgs() {
  return args;
}