set(UNITTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
macro(yul2llvm_gtest_suite target)
  target_link_libraries(${target} PRIVATE ${GTEST_LIB_TARGETS} ${GTEST_EXE_TARGET})
  list(APPEND YUL2LLVM_UNITTESTS ${target})
  if (NOT UNITTEST_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(YUL2LLVM_UNITTESTS ${YUL2LLVM_UNITTESTS} PARENT_SCOPE)
  endif()
  gtest_discover_tests(${target})
endmacro()

if(Y2L_ENABLE_LLVM)
  add_executable(ConvertGMPToAPIntTest ConvertGMPToAPIntTest.cpp)
  target_link_libraries(ConvertGMPToAPIntTest PRIVATE libyul2llvm)
  yul2llvm_gtest_suite(ConvertGMPToAPIntTest)
endif()

add_subdirectory(ASTClassesTest)

add_custom_target(check-unit
  DEPENDS ${YUL2LLVM_UNITTESTS}
  COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --target test
)
