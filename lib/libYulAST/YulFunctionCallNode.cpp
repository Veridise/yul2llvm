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

  std::vector<llvm::Type *> funcArgTypes(
      numargs, llvm::Type::getIntNTy(*TheContext, 256));

  FT = llvm::FunctionType::get(llvm::Type::getIntNTy(*TheContext, 256),
                               funcArgTypes, false);

  F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage,
                             callee->getIdentfierValue(), TheModule.get());
}

llvm::Value *YulFunctionCallNode::emitStorageLoadIntrinsic() {
  assert(args.size() == 2);
  assert(args[0]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  assert(args[1]->expressionType ==
         YUL_AST_EXPRESSION_NODE_TYPE::YUL_AST_EXPRESSION_LITERAL);
  YulLiteralNode &lit0 = (YulLiteralNode &)(*(args[0]));
  YulLiteralNode &lit1 = (YulLiteralNode &)(*(args[1]));
  assert(lit0.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  assert(lit1.literalType == YUL_AST_LITERAL_NODE_TYPE::YUL_AST_LITERAL_STRING);
  YulStringLiteralNode &varLit = (YulStringLiteralNode &)(*(args[0]));
  YulStringLiteralNode &typeLit = (YulStringLiteralNode &)(*(args[1]));
  auto fieldIt = std::find(structFieldOrder.begin(), structFieldOrder.end(),
                           varLit.to_string());
  assert(fieldIt != structFieldOrder.end());
  int structIndex = fieldIt - structFieldOrder.begin();
  llvm::SmallVector<llvm::Value *> indices;
  indices.push_back(
      llvm::ConstantInt::get(*TheContext, llvm::APInt(32, 0, false)));
  indices.push_back(
      llvm::ConstantInt::get(*TheContext, llvm::APInt(32, structIndex, false)));
  llvm::Value *ptr = Builder->CreateGEP(selfType, (llvm::Value *)self, indices,
                                        "ptr_self_" + varLit.to_string());
  /**
   * @todo fix all bit widths;
   */
  return Builder->CreateLoad(llvm::Type::getIntNTy(*TheContext, 256), ptr,
                             "self_" + varLit.to_string());
}

llvm::Value *YulFunctionCallNode::codegen(llvm::Function *enclosingFunction) {
  if (callee->getIdentfierValue() == "pyul_storage_var_load") {
    return emitStorageLoadIntrinsic();
  }
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