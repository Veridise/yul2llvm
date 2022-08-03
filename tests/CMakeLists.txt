if (GTest_FOUND)
  message(STATUS "gtest found, enabling unit tests...")
  include(GoogleTest)

  if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.20")
    set(GTEST_LIB_TARGETS "GTest::gtest;GTest::gmock")
    set(GTEST_EXE_TARGET GTest::gtest_main)
  else()
    set(GTEST_LIB_TARGETS GTest::GTest)
    set(GTEST_EXE_TARGET GTest::Main)
  endif()

  add_subdirectory(cpp)
else()
  message(WARNING "gtest not found, unit tests will not be run")
endif()

find_program(LLVM_EXTERNAL_LIT NAMES lit llvm-lit ${LLVM_EXTERNAL_LIT})
if (LLVM_EXTERNAL_LIT STREQUAL "LLVM_EXTERNAL_LIT-NOTFOUND")
  message(WARNING "lit not found, C++-only lit tests will not be run")
else()
  message(STATUS "Using lit: ${LLVM_EXTERNAL_LIT}")
  configure_lit_site_cfg(
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
    MAIN_CONFIG
    ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
  )

  add_lit_testsuite(check-lit "Running the lit-based regression tests"
    ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS yul2llvm_cpp
  )
endif()
