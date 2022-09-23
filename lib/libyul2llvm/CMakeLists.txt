add_library(libyul2llvm TranslateYulToLLVM.cpp)
target_include_directories(libyul2llvm PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(libyul2llvm PUBLIC
  libYulAST
  nlohmann_json::nlohmann_json
  LLVMCore
  LLVMPasses
)
