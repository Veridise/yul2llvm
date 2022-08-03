#pragma once

#include <nlohmann/json.hpp>

namespace yul2llvm {

class TranslateYulToLLVM {
    /// TODO: this is a placeholder interface, please replace this with the real
    /// interface
    void run(const nlohmann::json &yulAst);
};

} // namespace yul2llvm
