#pragma once
#include <regex>
#include <libYulAST/YulConstants.h>

namespace yulast{
class IntrinsicPatternMatcher{
    std::regex readFromStorageOffsetRegex;
    std::regex readFromStorageDynamicRegex;
    std::regex updateStorageOffsetfRegex;
    std::regex updateStorageDynamicRegex;

    public:
        IntrinsicPatternMatcher();
        YUL_INTRINSIC_ID getYulIntriniscType(std::string_view name);
        int readFromStorageOffsetGetOffset(std::string_view name);
        std::string readFromStorageOffsetGetType(std::string_view name);
        std::string readFromStorageDynamicGetType(std::string_view name);
        
        int updateStorageOffsetGetOffset(std::string_view name);
        std::string updateStorageOffsetGetFromType(std::string_view name);
        std::string updateStorageOffsetGetToType(std::string_view name);
        std::string updateStorageDynamicGetFromType(std::string_view name);
        std::string updateStorageDynamicGetToType(std::string_view name);
};
}