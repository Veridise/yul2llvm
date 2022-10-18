#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicHelper.h>

YulIntrinsicHelper::YulIntrinsicHelper(LLVMCodegenVisitor &v) : visitor(v) {}

void printFunction(llvm::Function *enclosingFunction) {
  enclosingFunction->print(llvm::outs());
  llvm::outs() << "\n";
}

llvm::Function *
YulIntrinsicHelper::getOrCreateFunction(std::string name,
                                        llvm::FunctionType *FT) {
  llvm::Function *F = nullptr;
  F = visitor.getModule().getFunction(name);
  if (F)
    return F;
  else {
    F = llvm::Function::Create(FT,
                               llvm::Function::LinkageTypes::ExternalLinkage,
                               name, visitor.getModule());
    return F;
  }
}

llvm::Value *YulIntrinsicHelper::getPointerToStorageVarByName(
    std::string name, llvm::Instruction *insertPoint) {
  llvm::IRBuilder<> tempBuilder(insertPoint);
  auto structFieldOrder = visitor.currentContract->getStructFieldOrder();
  auto typeMap = visitor.currentContract->getVarTypeMap();
  auto fieldIt =
      std::find(structFieldOrder.begin(), structFieldOrder.end(), name);
  assert(fieldIt != structFieldOrder.end());
  int structIndex = fieldIt - structFieldOrder.begin();
  llvm::SmallVector<llvm::Value *> indices;
  llvm::Value *self = tempBuilder.CreatePointerCast(
      visitor.getSelfArg(), visitor.getSelfType()->getPointerTo());
  indices.push_back(
      llvm::ConstantInt::get(visitor.getContext(), llvm::APInt(32, 0, false)));
  indices.push_back(llvm::ConstantInt::get(
      visitor.getContext(), llvm::APInt(32, structIndex, false)));
  llvm::Value *ptr = tempBuilder.CreateGEP(visitor.getSelfType(), self, indices,
                                           "ptr_self_" + name);
  return ptr;
}

llvm::Type *YulIntrinsicHelper::getTypeByTypeName(llvm::StringRef type) {
  auto typeInfoMap = visitor.currentContract->getTypeInfoMap();
  std::regex uintTypeRegex(R"(^t_uint(\d+)$)");
  std::regex bytesTypeRegex(R"(^t_bytes(\d+)$)");
  std::smatch match;
  std::string typeStr= type.str();;
  int bitWidth=0;
  if(typeInfoMap.find(type.str()) != typeInfoMap.end()){
    bitWidth = visitor.currentContract->getTypeInfoMap()[type.str()].size * 8;
  } else if(std::regex_match(typeStr, match, uintTypeRegex)){
      if(llvm::StringRef(match[1].str()).getAsInteger(10, bitWidth)){
        //@todo raise runtime error
        assert(false && "could not parse bitwidth while inferring datatype");
      }
  } else if(std::regex_match(typeStr, match, bytesTypeRegex)){
    if(llvm::StringRef(match[1].str()).getAsInteger(10, bitWidth)){
      //@todo raise runtime error
      assert(false && "could not parse bitwidth while inferring datatype");
    }
    bitWidth = bitWidth*8;
  } else {
    //@todo raise runtime error
    assert(false && "type not found in typeinfomap and could not infer");
  }
  return llvm::Type::getIntNTy(visitor.getContext(), bitWidth);

}

llvm::StringRef
YulIntrinsicHelper::getStorageVarYulTypeByName(llvm::StringRef name) {
  llvm::StringRef type(visitor.currentContract->getVarTypeMap()[name].type);
  auto &typeMap = visitor.currentContract->getTypeInfoMap();
  auto typeInfo = typeMap.find(type);
  assert(typeInfo != typeMap.end() && "Unreconized type of a variable");
  return type;
}



llvm::FunctionType *
YulIntrinsicHelper::getFunctionType(YulFunctionCallNode &node,
                                    llvm::SmallVector<llvm::Value *> &argsV) {
  auto funcArgTypes = getFunctionArgTypes(node.getCalleeName(), argsV);
  auto retType = getReturnType(node.getCalleeName());
  return llvm::FunctionType::get(retType, funcArgTypes, false);
}

llvm::Type *YulIntrinsicHelper::getReturnType(llvm::StringRef calleeName) {
  if (calleeName == "revert")
    return llvm::Type::getVoidTy(visitor.getContext());
  else if (calleeName == "__pyul_map_index")
    return llvm::Type::getIntNPtrTy(visitor.getContext(), 256);
  else if (calleeName == "pyul_storage_var_update")
    return llvm::Type::getVoidTy(visitor.getContext());
  else if (calleeName == "__pyul_storage_var_dynamic_load")
    return llvm::Type::getIntNTy(visitor.getContext(), 256);
  else if (calleeName.startswith("abi_encode_"))
    return llvm::Type::getIntNPtrTy(visitor.getContext(), 256);
  return llvm::Type::getIntNTy(visitor.getContext(), 256);
}

llvm::SmallVector<llvm::Type *> YulIntrinsicHelper::getFunctionArgTypes(
    std::string calleeName, llvm::SmallVector<llvm::Value *> &argsV) {
  llvm::SmallVector<llvm::Type *> funcArgTypes;
  if (calleeName == "__pyul_storage_var_load") {
    funcArgTypes.push_back(llvm::Type::getIntNPtrTy(visitor.getContext(), 256));
  } else if (calleeName == "__pyul_storage_var_update") {
    funcArgTypes.push_back(llvm::Type::getIntNPtrTy(visitor.getContext(), 256));
    funcArgTypes.push_back(llvm::Type::getIntNPtrTy(visitor.getContext(), 256));
  } else if (!calleeName.compare("__pyul_map_index")) {
    funcArgTypes.push_back(llvm::Type::getIntNPtrTy(visitor.getContext(), 256));
    funcArgTypes.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
  } else if (!calleeName.compare("__pyul_storage_var_dynamic_load")) {
    funcArgTypes.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
    funcArgTypes.push_back(llvm::Type::getIntNTy(visitor.getContext(), 256));
  } else {
    for (auto &arg : argsV) {
      funcArgTypes.push_back(arg->getType());
    }
  }
  return funcArgTypes;
}

LLVMCodegenVisitor &YulIntrinsicHelper::getVisitor() { return visitor; }
