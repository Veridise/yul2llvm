add_subdirectory(libYulAST)

if(Y2L_ENABLE_LLVM)
  add_subdirectory(libyul2llvm)
  add_subdirectory(yul2llvm_cpp)
endif()
