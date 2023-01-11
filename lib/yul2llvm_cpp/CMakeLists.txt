add_executable(yul2llvm_cpp yul2llvm_cpp.cpp)
target_link_libraries(yul2llvm_cpp PRIVATE LLVMSupport libyul2llvm)
set_target_properties(yul2llvm_cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
llvm_update_compile_flags(yul2llvm_cpp)
target_compile_options(yul2llvm_cpp PRIVATE -fexceptions)  # needed by gmpcxx

install(TARGETS yul2llvm_cpp
  EXPORT Yul2LLVMTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
