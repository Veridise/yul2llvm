macro(yul2llvm_gtest_suite target)
  gtest_discover_tests(${target})
  list(APPEND YUL2LLVM_UNITTESTS ${target})
endmacro()

add_custom_target(check-unit
  DEPENDS ${YUL2LLVM_UNITTESTS}
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target test
)

add_subdirectory(ASTClassesTest)
