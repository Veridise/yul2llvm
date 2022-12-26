#include <cassert>
#include <libYulAST/IntrinsicPatterns.h>
#include <iostream>
namespace yulast {
IntrinsicPatternMatcher::IntrinsicPatternMatcher()
    : readFromStorageOffsetRegex(READ_FROM_STORAGE_OFFSET_REGEX_LIT),
      readFromStorageDynamicRegex(READ_FROM_STORAGE_DYNAMIC_REGEX_LIT),
      readFromStorageReferenceRegex(READ_FROM_STORAGE_REFERENCE_REGEX_LIT),
      updateStorageOffsetfRegex(UPDATE_STORAGE_OFFSET_REGEX_LIT),
      updateStorageDynamicRegex(UPDATE_STORAGE_DYNAMIC_REGEX_LIT) {}

YUL_INTRINSIC_ID
IntrinsicPatternMatcher::getYulIntriniscType(std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, readFromStorageOffsetRegex)) {
    return YUL_INTRINSIC_ID::READ_FROM_STORAGE_OFFSET;
  } else if (std::regex_match(nameStr, match, readFromStorageDynamicRegex)) {
    return YUL_INTRINSIC_ID::READ_FROM_STORAGE_DYNAMIC;
  } else if (std::regex_match(nameStr, match, readFromStorageReferenceRegex)) {
    return YUL_INTRINSIC_ID::READ_FROM_STORAGE_REFERENCE;
  } else if (std::regex_match(nameStr, match, updateStorageOffsetfRegex)) {
    return YUL_INTRINSIC_ID::UPDATE_STORAGE_OFFSET;
  } else if (std::regex_match(nameStr, match, updateStorageDynamicRegex)) {
    return YUL_INTRINSIC_ID::UPDATE_STORAGE_DYNAMIC;
  }
  return YUL_INTRINSIC_ID::INVALID_INTRINSIC_ID;
}

int IntrinsicPatternMatcher::readFromStorageOffsetGetOffset(
    std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, readFromStorageOffsetRegex)) {
    return std::stoi(match[2].str());
  } else {
    assert(false && "Regex did not match");
  }
  return 0;
}

std::string
IntrinsicPatternMatcher::readFromStorageOffsetGetType(std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, readFromStorageOffsetRegex)) {
    return match[3].str();
  } else {
    assert(false && "Regex did not match");
  }
  return "";
}

std::string
IntrinsicPatternMatcher::readFromStorageDynamicGetType(std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, readFromStorageDynamicRegex)) {
    return match[1].str();
  } else {
    assert(false && "Regex did not match");
  }
  return "";
}

int IntrinsicPatternMatcher::updateStorageOffsetGetOffset(
    std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, updateStorageOffsetfRegex)) {
    return std::stoi(match[1].str());
  } else {
    assert(false && "Regex did not match");
  }
  return 0;
}

std::string
IntrinsicPatternMatcher::updateStorageOffsetGetFromType(std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, updateStorageOffsetfRegex)) {
    return match[2].str();
  } else {
    assert(false && "Regex did not match");
  }
  return "";
}

std::string
IntrinsicPatternMatcher::updateStorageOffsetGetToType(std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, updateStorageOffsetfRegex)) {
    return match[3].str();
  } else {
    assert(false && "Regex did not match");
  }
  return "";
}

std::string IntrinsicPatternMatcher::updateStorageDynamicGetFromType(
    std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, updateStorageDynamicRegex)) {
    return match[1].str();
  } else {
    assert(false && "Regex did not match");
  }
  return "";
}

std::string
IntrinsicPatternMatcher::updateStorageDynamicGetToType(std::string_view name) {
  std::smatch match;
  std::string nameStr(name);
  if (std::regex_match(nameStr, match, updateStorageDynamicRegex)) {
    return match[2].str();
  } else {
    assert(false && "Regex did not match");
  }
  return "";
}

ReadFromStorageOffsetResult
IntrinsicPatternMatcher::parseReadFromStorageOffset(std::string_view name) {
  ReadFromStorageOffsetResult res;
  res.offset = readFromStorageOffsetGetOffset(name);
  res.type = readFromStorageOffsetGetType(name);
  return res;
}

ReadFromStorageDynamicResult
IntrinsicPatternMatcher::parseReadFromStorageDynamic(std::string_view name) {
  ReadFromStorageDynamicResult res;
  res.type = readFromStorageDynamicGetType(name);
  return res;
}

UpdateStorageOffsetResult
IntrinsicPatternMatcher::parseUpdateStorageOffset(std::string_view name) {
  UpdateStorageOffsetResult res;
  res.offset = updateStorageOffsetGetOffset(name);
  res.fromType = updateStorageOffsetGetFromType(name);
  res.toType = updateStorageOffsetGetToType(name);
  return res;
}

UpdateStorageDynamicResult
IntrinsicPatternMatcher::parseUpdateStorageDynamic(std::string_view name) {
  UpdateStorageDynamicResult res;
  res.fromType = updateStorageDynamicGetFromType(name);
  res.toType = updateStorageDynamicGetToType(name);
  return res;
}
StructTypeResult
IntrinsicPatternMatcher::parseStructTypeFromStorageLayout(std::string_view name) {
  StructTypeResult res;
  std::string nameStr(name);
  std::regex regex(STRUCT_TYPE_STORAGE_LAYOUT_REGEX_LIT);
  std::smatch match;
  int match_success = std::regex_match(nameStr, match, regex);
  assert(match_success && "Struct match did not match regex");
  res.name = match[1].str();
  res.size = std::stoi(match[2].str());
  res.location = match[3].str();
  return res;
}

ConvertXToYResult
IntrinsicPatternMatcher::parseConvertXToY(std::string_view name) {
  ConvertXToYResult res;
  std::string nameStr(name);
  std::regex regex(CONVERT_TYPE_X_TO_TYPE_Y_REGEX_LIT);
  std::smatch match;
  int match_success = std::regex_match(nameStr, match, regex);
  assert(match_success && "ConvertXToY did not match regex");
  res.sourceType = match[1].str();
  res.sourceAddressSpace = match[2].str();
  res.isSourceTypePointer = match[3].matched;

  res.destType = match[4].str();
  res.destAddressSpace = match[5].str();
  res.isSourceTypePointer = match[6].matched;
  return res;
}

StructTypeResult
IntrinsicPatternMatcher::parseStructTypeFromYul(std::string_view name) {
  StructTypeResult res;
  std::string nameStr(name);
  std::regex regex(STRUCT_TYPE_YUL_REGEX_LIT);
  std::smatch match;
  int match_success = std::regex_match(nameStr, match, regex);
  std::cout<<nameStr<<"\n";
  assert(match_success && "Yul struct type did not match regex");
  res.name = match[1].str();
  res.size = std::stoi(match[2].str());
  res.location = match[3].str();
  return res;
}

StructTypeResult
IntrinsicPatternMatcher::parseStructTypeFromAbi(std::string_view name) {
  StructTypeResult res;
  std::string nameStr(name);
  std::regex regex(STRUCT_TYPE_ABI_REGEX_LIT);
  std::smatch match;
  int match_success = std::regex_match(nameStr, match, regex);
  assert(match_success && "Yul struct type did not match regex");
  res.name = match[2].str();
  res.size = 0;
  res.location = "<unknown>";
  return res;
}

}; // namespace yulast
