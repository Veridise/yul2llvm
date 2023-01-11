#pragma once
#include <libYulAST/YulConstants.h>
#include <regex>

namespace yulast {
struct ReadFromStorageOffsetResult {
  std::string type;
  int offset;
};

struct ReadFromStorageDynamicResult {
  std::string type;
};

struct UpdateStorageOffsetResult {
  std::string fromType;
  std::string toType;
  int offset;
};

struct UpdateStorageDynamicResult {
  std::string fromType;
  std::string toType;
};

struct StructTypeResult {
  std::string name;
  std::string location;
  int size;
};

struct ConvertXToYResult {
  std::string sourceType;
  std::string sourceAddressSpace;
  bool isSourceTypePointer;
  std::string destType;
  std::string destAddressSpace;
  bool isDestTypePtr;
};

class IntrinsicPatternMatcher {
  std::regex readFromStorageOffsetRegex;
  std::regex readFromStorageDynamicRegex;
  std::regex readFromStorageReferenceRegex;
  std::regex updateStorageOffsetfRegex;
  std::regex updateStorageDynamicRegex;

public:
  IntrinsicPatternMatcher();
  YUL_INTRINSIC_ID getYulIntriniscType(std::string_view name);
  int readFromStorageOffsetGetOffset(std::string_view name);
  std::string readFromStorageOffsetGetType(std::string_view name);
  ReadFromStorageOffsetResult parseReadFromStorageOffset(std::string_view name);

  std::string readFromStorageDynamicGetType(std::string_view name);
  ReadFromStorageDynamicResult
  parseReadFromStorageDynamic(std::string_view name);

  int updateStorageOffsetGetOffset(std::string_view name);
  std::string updateStorageOffsetGetFromType(std::string_view name);
  std::string updateStorageOffsetGetToType(std::string_view name);
  UpdateStorageOffsetResult parseUpdateStorageOffset(std::string_view name);

  std::string updateStorageDynamicGetFromType(std::string_view name);
  std::string updateStorageDynamicGetToType(std::string_view name);
  UpdateStorageDynamicResult parseUpdateStorageDynamic(std::string_view name);
  StructTypeResult parseStructTypeFromStorageLayout(std::string_view name);

  ConvertXToYResult parseConvertXToY(std::string_view name);
  StructTypeResult parseStructTypeFromYul(std::string_view name);
  StructTypeResult parseStructTypeFromAbi(std::string_view name);
};
} // namespace yulast