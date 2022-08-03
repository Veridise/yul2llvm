add_executable(yul2llvm_cpp yul2llvm_cpp.cpp)
target_link_libraries(yul2llvm_cpp PRIVATE LLVMSupport libyul2llvm)
set_target_properties(yul2llvm_cpp PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

install(TARGETS yul2llvm_cpp
  EXPORT ${Y2L_EXPORT_TARGETS}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
