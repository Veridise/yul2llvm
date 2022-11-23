add_executable(ast-class-test
    ASTClassTest.cpp
)

target_link_libraries(ast-class-test PRIVATE
    libYulAST
    nlohmann_json::nlohmann_json
)
