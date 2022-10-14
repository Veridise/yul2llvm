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

llvm::Type *YulIntrinsicHelper::getTypeByTypeName(llvm::StringRef type){
  int bitWidth = visitor.currentContract->getTypeInfoMap()[type.str()].size*8;
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

llvm::Value *YulIntrinsicHelper::cleanup(llvm::Value *v,
                                         llvm::StringRef typeRef,
                                         llvm::Value *offset, 
                                         llvm::Instruction *insertPoint) {
  std::unique_ptr<llvm::IRBuilder<>> tempBuilder;
  llvm::IRBuilder<> *builder;
  if (insertPoint == nullptr)
    builder = &visitor.getBuilder();
  else {
    tempBuilder = std::make_unique<llvm::IRBuilder<>>(insertPoint);
    builder = tempBuilder.get();
  }
  std::regex typeRegex(R"(t_([a-z]+)([0-9]+))");
  std::smatch typeMatch;
  std::string typeStr = typeRef.str();
  bool found = std::regex_match(typeStr, typeMatch, typeRegex);
  if (!found) {
    //@todo refactor to raise runtime error.
    assert(false && "Type did not match pattern in cleanup");
  }
  std::string type = typeMatch[1].str();
  std::string bitWidthStr = typeMatch[2].str();
  int bitWidth;
  if (llvm::StringRef(bitWidthStr).getAsInteger(10, bitWidth)) {
    //@todo refactor to raise runtime error.
    assert(false && "Could not parse type bitwidth in cleanup");
  }

  if (type == "uint") {
    llvm::APInt bitmask(256, 0, false);
    bitmask.setBits(0, bitWidth);
    v = builder->CreateLShr(v, offset);
    return builder->CreateAnd(v, bitmask,
                              "cleaned_up_t_" + type + bitWidthStr + "_");
  } else if (type == "bytes") {
    bitWidth = bitWidth * 8;
    llvm::APInt bitmask(256, 0, false);
    bitmask.setBits(256 - bitWidth, 256);
    return builder->CreateAnd(v, bitmask,
                              "cleaned_up_t_" + type + bitWidthStr + "_");
  }
  assert(false && "Cleanupt unrecognized type");
  return nullptr;
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
