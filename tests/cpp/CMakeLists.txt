macro(yul2llvm_gtest_suite target)
  gtest_discover_tests(${target})
  list(APPEND YUL2LLVM_UNITTESTS ${target})
endmacro()

add_executable(Test_Example
  Example_Test.cpp
)
target_link_libraries(Test_Example PRIVATE
  libyul2llvm ${GTEST_LIB_TARGETS} ${GTEST_EXE_TARGET}
)
yul2llvm_gtest_suite(Test_Example)

add_custom_target(check-unit
  DEPENDS ${YUL2LLVM_UNITTESTS}
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target test
)

add_subdirectory(ASTClassesTest)