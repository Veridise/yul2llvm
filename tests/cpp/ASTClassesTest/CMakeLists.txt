add_executable(ast-class-test
    ASTClassTest.cpp
)

target_link_libraries(ast-class-test PRIVATE
    libYulAST
    nlohmann_json::nlohmann_json
)

yul2llvm_gtest_suite(ast-class-test)
