#include <libyul2llvm/YulASTVisitor/CodegenVisitor.h>
#include <libyul2llvm/YulASTVisitor/IntrinsicHelper.h>

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
    std::vector<std::string> namePath, llvm::Instruction *insertPoint) {
  llvm::IRBuilder<> tempBuilder(insertPoint);
  std::vector<int> rawIndices =
      visitor.currentContract->getIndexPathByName(namePath);
  llvm::SmallVector<llvm::Value *> indices;
  llvm::Value *self = tempBuilder.CreatePointerCast(
      visitor.getSelfArg(),
      visitor.getSelfType()->getPointerTo(STORAGE_ADDR_SPACE));
  for (int index : rawIndices) {
    indices.push_back(llvm::ConstantInt::get(visitor.getContext(),
                                             llvm::APInt(32, index, false)));
  }
  llvm::Value *ptr =
      tempBuilder.CreateGEP(visitor.getSelfType(), self, indices,
                            "ptr_self_" + getValueNameFromNamePath(namePath));
  return ptr;
}

llvm::Type *YulIntrinsicHelper::getTypeByTypeName(llvm::StringRef type,
                                                  const int addrSpaceId = 0) {
  auto &typeInfoMap = visitor.currentContract->getTypeInfoMap();
  std::regex uintTypeRegex(R"(^t_uint(\d+)$)");
  std::regex bytesTypeRegex(R"(^t_bytes(\d+)$)");
  std::smatch match;
  std::string typeStr = type.str();
  int bitWidth = 0;
  if (typeInfoMap.find(type.str()) != typeInfoMap.end()) {
    bitWidth = visitor.currentContract->getTypeInfoMap()[type.str()].size * 8;
  } else if (std::regex_match(typeStr, match, uintTypeRegex)) {
    if (llvm::StringRef(match[1].str()).getAsInteger(10, bitWidth)) {
      //@todo raise runtime error
      assert(false && "could not parse bitwidth while inferring datatype");
    }
  } else if (std::regex_match(typeStr, match, bytesTypeRegex)) {
    if (llvm::StringRef(match[1].str()).getAsInteger(10, bitWidth)) {
      //@todo raise runtime error
      assert(false && "could not parse bitwidth while inferring datatype");
    }
    bitWidth = bitWidth * 8;
  } else if (type.startswith("t_array")) {
    return visitor.getDefaultType()->getPointerTo(addrSpaceId);
  } else {
    //@todo raise runtime error
    assert(false && "type not found in typeinfomap and could not infer");
  }
  return llvm::Type::getIntNTy(visitor.getContext(), bitWidth);
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
  return llvm::Type::getIntNTy(visitor.getContext(), 256);
}

llvm::SmallVector<llvm::Type *> YulIntrinsicHelper::getFunctionArgTypes(
    std::string_view calleeName, llvm::SmallVector<llvm::Value *> &argsV) {
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

std::string YulIntrinsicHelper::getValueNameFromNamePath(
    std::vector<std::string> namePath) {
  assert(!namePath.empty() && "Empty name path provided");
  std::string name = namePath[0];
  for (auto it = namePath.begin() + 1; it != namePath.end(); it++) {
    name = name + "_" + *it;
  }
  return name;
}
