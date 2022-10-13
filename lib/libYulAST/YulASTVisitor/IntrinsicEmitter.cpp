#include <libYulAST/YulASTVisitor/CodegenVisitor.h>
#include <libYulAST/YulASTVisitor/IntrinsicHelper.h>

bool YulIntrinsicHelper::isFunctionCallIntrinsic(llvm::StringRef calleeName) {

  if (calleeName == "checked_add_t_uint256") {
    return true;
  } else if (calleeName.startswith("mstore")) {
    return true;
  } else if (calleeName == "add") {
    return true;
  } else if (calleeName == "sub") {
    return true;
  } else if (calleeName == "shl") {
    return true;
  } else if (calleeName == "and") {
    return true;
  } else if (calleeName == "allocate_unbounded") {
    return true;
  } else if (calleeName.startswith("memory_array_index_access_")) {
    return true;
  } else if (calleeName.startswith("read_from_memory")) {
    return true;
  } else if (calleeName.startswith("write_to_memory")) {
    return true;
  } else if (calleeName.startswith("convert_t_rational_")) {
    return true;
  } else if (calleeName.startswith("byte")) {
    return true;
  }
  return false;
}

llvm::Value *
YulIntrinsicHelper::handleIntrinsicFunctionCall(YulFunctionCallNode &node) {
  std::string calleeNameStr = node.getCalleeName();
  llvm::StringRef calleeName(calleeNameStr);
  if (!calleeName.compare("checked_add_t_uint256")) {
    return handleAddFunctionCall(node);
  } else if (calleeName.startswith("mstore")) {
    return handleMStoreFunctionCall(node);
  } else if (!calleeName.compare("add")) {
    return handleAddFunctionCall(node);
  } else if (!calleeName.compare("sub")) {
    return handleSubFunctionCall(node);
  } else if (!calleeName.compare("shl")) {
    return handleShl(node);
  } else if (!calleeName.compare("and")) {
    return handleAnd(node);
  } else if (calleeName == "allocate_unbounded") {
    return handleAllocateUnbounded(node);
  } else if (calleeName.startswith("memory_array_index_access_")) {
    return handleArrayIndexAccess(node);
  } else if (calleeName.startswith("read_from_memory")) {
    return handleReadFromMemory(node);
  } else if (calleeName.startswith("write_to_memory")) {
    return handleWriteToMemory(node);
  } else if (calleeName.startswith("convert_t_rational_")) {
    return handleConvertRationalXByY(node);
  } else if (calleeName.startswith("byte")) {
    return handleByte(node);
  }
  return nullptr;
}

bool YulIntrinsicHelper::skipDefinition(llvm::StringRef calleeName) {
  if (calleeName.startswith("abi_encode_")) {
    return true;
  } else if (calleeName.startswith("abi_decode_tuple_")) {
    return true;
  } else if (calleeName.startswith("finalize_allocation")) {
    return true;
  } else if (calleeName.startswith("memory_array_index_access_")) {
    return true;
  } else if (calleeName.startswith("read_from_memory")) {
    return true;
  } else if (calleeName.startswith("write_to_memory")) {
    return true;
  } else if (calleeName.startswith("allocate_memory")) {
    return true;
  } else if (calleeName.startswith("convert_t_rational_")) {
    return true;
  } else if (calleeName == "checked_add_t_uint256") {
    return true;
  } else if (calleeName.startswith("mstore")) {
    return true;
  } else if (calleeName == "add") {
    return true;
  } else if (calleeName == "sub") {
    return true;
  } else if (calleeName == "shl") {
    return true;
  } else if (calleeName == "and") {
    return true;
  } else if (calleeName == "byte") {
    return true;
  } else if (calleeName.startswith("panic_error")) {
    return true;
  } else if (calleeName.startswith("revert")) {
    return true;
  } else if (calleeName.startswith("read_from_storage")) {
    return true;
  } else
    return false;
}

llvm::Value *YulIntrinsicHelper::handleByte(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 &&
         "Unexpected number of args in byte(x, y)");
  llvm::Value *v2 = visitor.visit(*node.getArgs()[1]);
  if (v2->getType()->isIntegerTy()) {
    return visitor.getBuilder().CreateIntCast(
        v2, llvm::Type::getInt8Ty(visitor.getContext()), false,
        "byte_" + v2->getName());
  }
  return nullptr;
}

llvm::Value *YulIntrinsicHelper::handleAnd(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 && "And not called with 2 arguments");
  llvm::Value *lhs = visitor.visit(*node.getArgs()[0]);
  llvm::Value *rhs = visitor.visit(*node.getArgs()[1]);
  return visitor.getBuilder().CreateAnd(lhs, rhs);
}

llvm::Value *
YulIntrinsicHelper::handleConvertRationalXByY(YulFunctionCallNode &node) {
  std::regex convertCallRegex(
      R"(^convert_t_rational(_minus)?_([0-9]+)_by(_minus)?_([0-9]+)_to_t_([a-z]+)([0-9]+)$)");
  std::smatch match;
  std::string calleeName = node.getCalleeName();
  bool found = std::regex_match(calleeName, match, convertCallRegex);
  assert(found && "convert_t_rational did not match");
  if (found && match.size() == 7) {
    std::string strNumerator = match[2].str();
    std::string strDenominator = match[4].str();
    llvm::StringRef srNumerator(strNumerator);
    llvm::StringRef srDenominator(strDenominator);
    llvm::APInt numerator, denominator, quotient, reminder;
    if (srNumerator.getAsInteger(10, numerator)) {
      llvm::outs() << numerator << " " << denominator;
      assert(false && "Could not parse numerator in convert_t_rational");
    }
    if (srDenominator.getAsInteger(10, denominator) && denominator != 0) {
      assert(false && "Could not parse donominator in convert_t_rational");
    }
    if (match[1].matched)
      numerator = -numerator;
    if (match[3].matched)
      denominator = -denominator;
    // assert integral
    llvm::APInt::udivrem(numerator, denominator, quotient, reminder);
    quotient = quotient.sext(256);
    assert(reminder == 0 && "Non integreal types not implemented");
    return llvm::ConstantInt::get(visitor.getDefaultType(), quotient);
  }
  assert(false &&
         "convert_t_rational either did not match regex or wrong count");
  return nullptr;
}

llvm::Value *
YulIntrinsicHelper::handleReadFromMemory(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 1 &&
         "Wrong number of arguments read_from_memory_t_x call");
  std::regex readCallNameRegex(R"(^read_from_memory(t_[a-z]+)(\d+))");
  std::smatch match;
  std::string calleeName = node.getCalleeName();
  if (std::regex_match(calleeName, match, readCallNameRegex)) {
    assert(match.size() == 3 && "read_from_memory does not match the pattern");
    std::string type = match[1].str();
    std::string widthStr = match[2].str();
    llvm::Value *pointer = visitor.visit(*node.getArgs()[0]);
    auto &builder = visitor.getBuilder();
    llvm::Value *loadedWord =
        builder.CreateLoad(visitor.getDefaultType(), pointer, "arr_load");
    return cleanup(loadedWord, type + widthStr,
                      llvm::ConstantInt::get(visitor.getDefaultType(), 0, 10));
  }
  assert(false && "regex did not match read_from_memoryt");
  return nullptr;
}
llvm::Value *
YulIntrinsicHelper::handleWriteToMemory(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 &&
         "Wrong number of arguments write_to_memory_t_x call");
  std::regex readCallNameRegex(R"(^write_to_memory_(t_[a-z]+)(\d+))");
  std::smatch match;
  std::string calleeName = node.getCalleeName();
  if (std::regex_match(calleeName, match, readCallNameRegex)) {
    assert(match.size() == 3 && "write_to_memory does not match the pattern");
    std::string type = match[1].str();
    std::string widthStr = match[2].str();
    llvm::Value *pointer = visitor.visit(*node.getArgs()[0]);
    llvm::Value *valueToStore = visitor.visit(*node.getArgs()[1]);
    auto &builder = visitor.getBuilder();
    llvm::Value *cleanedUpValue = cleanup(valueToStore, type + widthStr, llvm::ConstantInt::get(visitor.getDefaultType(), 0, 10));
    builder.CreateStore(cleanedUpValue, pointer);
  }
  return nullptr;
}

/**
 * @brief
 * memory_array_index_access_t_array$_t_uint256_$dyn_memory_ptr(arrayptr, idx)
 * rewritten to
 * %0 = getelementptr [0 x i256], [0 x i256]* %arr_ptr, i32 0, i256 %index
 *
 * @param node
 * @return llvm::Value*
 */
llvm::Value *
YulIntrinsicHelper::handleArrayIndexAccess(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 &&
         "Wrong number of arguments in memory_array_index_access");
  auto &builder = visitor.getBuilder();
  llvm::ArrayType *arrayType =
      llvm::ArrayType::get(llvm::Type::getIntNTy(visitor.getContext(), 256), 0);
  llvm::Value *array = visitor.visit(*node.getArgs()[0]);
  llvm::Value *idx = visitor.visit(*node.getArgs()[1]);
  auto pointerCast =
      builder.CreateCast(llvm::Instruction::CastOps::IntToPtr, array,
                         llvm::Type::getIntNPtrTy(visitor.getContext(), 256),
                         "ptr_" + array->getName());
  auto arrayCast = builder.CreatePointerCast(
      pointerCast, arrayType->getPointerTo(), "arr_" + array->getName());
  auto indices = visitor.getLLVMValueVector({0});
  indices.push_back(idx);
  auto elementPtr = builder.CreateGEP(arrayType, arrayCast, indices);
  return elementPtr;
}

llvm::Value *
YulIntrinsicHelper::handleAllocateUnbounded(YulFunctionCallNode &node) {
  return visitor.CreateEntryBlockAlloca(
      visitor.currentFunction, "alloc_unbounded",
      llvm::Type::getIntNTy(visitor.getContext(), 256));
}
llvm::Value *YulIntrinsicHelper::handleShl(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 &&
         "Incorrect number of args in shl instruction handler");
  auto &builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  return builder.CreateShl(v1, v2);
}

/**
 * @brief Contract: The caller confirms that only one of the value is pointer
 * and other is a primitive type.
 *
 * @param v1
 * @param v2
 * @return llvm::Value*
 */
llvm::Value *YulIntrinsicHelper::handlePointerAdd(llvm::Value *v1,
                                                  llvm::Value *v2) {
  llvm::Value *ptr, *primitive;
  assert((v1->getType()->isPointerTy() ^ v2->getType()->isPointerTy()) &&
         "Both values are pointer in handlePointerArithmetic");
  ptr = v1->getType()->isPointerTy() ? v1 : v2;
  primitive = v1->getType()->isPointerTy() ? v2 : v1;
  assert(primitive->getType()->isIntegerTy() &&
         "primitive is not integer in pointer addition");
  llvm::Type *bytePtrType = llvm::Type::getInt8PtrTy(visitor.getContext());
  llvm::ArrayType *arrayfiedType = llvm::ArrayType::get(bytePtrType, 0);
  // This cast is required because we GEP needs a container type
  ptr = visitor.getBuilder().CreatePointerCast(
      ptr, arrayfiedType->getPointerTo(), "arr_" + ptr->getName());
  llvm::SmallVector<llvm::Value *> index = {primitive};
  return visitor.getBuilder().CreateGEP(arrayfiedType, ptr, index,
                                        ptr->getName() + "[" +
                                            primitive->getName() + "]");
}
/**
 * @brief Here the contract is that atleast one of the argument is
 * pointer type. This function handles two cases seperately
 * 1. If only one is pointer
 * 2. If both are pointers
 *
 * @param v1
 * @param v2
 * @return llvm::Value*
 */
llvm::Value *YulIntrinsicHelper::handlePointerSub(llvm::Value *v1,
                                                  llvm::Value *v2) {
  assert(v1->getType()->isPointerTy() ||
         v2->getType()->isPointerTy() && "Both values are not pointers");
  llvm::Value *ptr, *primitive;
  // if only one is prmitive
  if (v1->getType()->isPointerTy() ^ v2->getType()->isPointerTy()) {
    ptr = v1->getType()->isPointerTy() ? v1 : v2;
    primitive = v1->getType()->isPointerTy() ? v2 : v1;
    assert(primitive->getType()->isIntegerTy() &&
           "Non integer sub from prmitive");
    llvm::Value *negated = visitor.getBuilder().CreateMul(
        primitive,
        llvm::ConstantInt::get(
            primitive->getType(),
            llvm::APInt(-1, primitive->getType()->getIntegerBitWidth())));
    return handlePointerAdd(ptr, negated);
  } else {
    assert(v1->getType() == v2->getType() &&
           "pointer diff between different pointers");
    llvm::Type *destType = llvm::Type::getIntNTy(
        visitor.getContext(),
        visitor.getModule().getDataLayout().getPointerSize());
    llvm::Value *castedV1 = visitor.getBuilder().CreateCast(
        llvm::Instruction::CastOps::PtrToInt, v1, destType, "v1");
    llvm::Value *castedV2 = visitor.getBuilder().CreateCast(
        llvm::Instruction::CastOps::PtrToInt, v2, destType, "v2");
    return visitor.getBuilder().CreateSub(castedV1, castedV2);
  }
}

llvm::Value *
YulIntrinsicHelper::handleAddFunctionCall(YulFunctionCallNode &node) {
  llvm::IRBuilder<> &Builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  // Only one is pointer other is a scalar type
  if (v1->getType()->isPointerTy() ^ v2->getType()->isPointerTy()) {
    return handlePointerAdd(v1, v2);
  } // both values are same type, make sure they are not both pointer type
  else if (v1->getType()->isPointerTy() && v2->getType()->isPointerTy()) {
    assert(false && "pointer arithmetic with both pointers");
  } // both scalars
  return Builder.CreateAdd(v1, v2);
}

llvm::Value *
YulIntrinsicHelper::handleSubFunctionCall(YulFunctionCallNode &node) {
  llvm::IRBuilder<> &Builder = visitor.getBuilder();
  llvm::Value *v1, *v2;
  v1 = visitor.visit(*node.getArgs()[0]);
  v2 = visitor.visit(*node.getArgs()[1]);
  if (v1->getType()->isPointerTy() || v2->getType()->isPointerTy()) {
    return handlePointerSub(v1, v2);
  }
  return Builder.CreateSub(v1, v2);
}

llvm::Value *
YulIntrinsicHelper::handleMStoreFunctionCall(YulFunctionCallNode &node) {
  assert(node.getArgs().size() == 2 &&
         "Incorrect number of arguments to mstore");
  std::regex mstoreFunNameRegex(R"(mstore([0-9]*))");
  std::smatch match;
  std::string functionName = node.getCalleeName();
  llvm::Value *val, *ptr;
  ptr = visitor.visit(*(node.getArgs()[0]));
  val = visitor.visit(*(node.getArgs()[1]));
  //@todo Remove this cast when abi_decode_is done
  if (!ptr->getType()->isPointerTy())
    ptr = visitor.getBuilder().CreateIntToPtr(ptr,
                                              val->getType()->getPointerTo());
  bool found = std::regex_search(functionName, match, mstoreFunNameRegex);
  if (found) {
    if (match[1] != "") {
      std::string bitWidthStr = match[1].str();
      int bitWidth;
      if (llvm::StringRef(bitWidthStr).getAsInteger(10, bitWidth)) {
        //@todo refactor into raising runtime error
        assert(false && "cannot parse bitwidth");
      }
      ptr = visitor.getBuilder().CreatePointerCast(
          ptr, llvm::Type::getIntNPtrTy(visitor.getContext(), bitWidth),
          "truncated_" + ptr->getName());
    }
    visitor.getBuilder().CreateStore(val, ptr);
  }
  return nullptr;
}
