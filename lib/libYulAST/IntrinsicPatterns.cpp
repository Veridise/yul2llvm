#include <libYulAST/IntrinsicPatterns.h>
#include <cassert>

namespace yulast
{
  IntrinsicPatternMatcher::IntrinsicPatternMatcher() :
    readFromStorageOffsetRegex(READ_FROM_STORAGE_OFFSET_REGEX_LIT),
    readFromStorageDynamicRegex(READ_FROM_STORAGE_DYNAMIC_REGEX_LIT),
    updateStorageOffsetfRegex(UPDATE_STORAGE_OFFSET_REGEX_LIT),
    updateStorageDynamicRegex(UPDATE_STORAGE_DYNAMIC_REGEX_LIT){
  }

  YUL_INTRINSIC_ID IntrinsicPatternMatcher::
                getYulIntriniscType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, readFromStorageOffsetRegex )){
        return YUL_INTRINSIC_ID::READ_FROM_STORAGE_OFFSET;
    } else if(std::regex_match(nameStr, match, readFromStorageDynamicRegex )){
        return YUL_INTRINSIC_ID::READ_FROM_STORAGE_DYNAMIC;
    } else if(std::regex_match(nameStr, match, updateStorageOffsetfRegex )){
        return YUL_INTRINSIC_ID::UPDATE_STORAGE_OFFSET;
    } else if(std::regex_match(nameStr, match, updateStorageDynamicRegex )){
        return YUL_INTRINSIC_ID::UPDATE_STORAGE_DYNAMIC;
    }
    return YUL_INTRINSIC_ID::INVALID_INTRINSIC_ID;
  }  

  int IntrinsicPatternMatcher::readFromStorageOffsetGetOffset(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, readFromStorageOffsetRegex)){
        return std::stoi(match[2].str());
    } else {
        assert(false && "Regex did not match");
    }
    return 0;
  }

  std::string IntrinsicPatternMatcher::readFromStorageOffsetGetType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, readFromStorageOffsetRegex)){
        return match[3].str();
    } else {
        assert(false && "Regex did not match");
    }
    return "";
  }

  std::string IntrinsicPatternMatcher::readFromStorageDynamicGetType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, readFromStorageDynamicRegex)){
        return match[1].str();
    } else {
        assert(false && "Regex did not match");
    }
    return "";
  }

  int IntrinsicPatternMatcher::updateStorageOffsetGetOffset(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, updateStorageOffsetfRegex)){
        return std::stoi(match[1].str());
    } else {
        assert(false && "Regex did not match");
    }
    return 0;
  }

  std::string IntrinsicPatternMatcher::updateStorageOffsetGetFromType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, updateStorageOffsetfRegex)){
        return match[2].str();
    } else {
        assert(false && "Regex did not match");
    }
    return "";
  }

  std::string IntrinsicPatternMatcher::updateStorageOffsetGetToType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, updateStorageOffsetfRegex)){
        return match[3].str();
    } else {
        assert(false && "Regex did not match");
    }
    return "";
  }

  std::string IntrinsicPatternMatcher::updateStorageDynamicGetFromType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, updateStorageDynamicRegex)){
        return match[1].str();
    } else {
        assert(false && "Regex did not match");
    }
    return "";
  }

  std::string IntrinsicPatternMatcher::updateStorageDynamicGetToType(std::string_view name){
    std::smatch match;
    std::string nameStr(name);
    if(std::regex_match(nameStr, match, updateStorageDynamicRegex)){
        return match[2].str();
    } else {
        assert(false && "Regex did not match");
    }
    return "";
  }

}; // namespace yulast
