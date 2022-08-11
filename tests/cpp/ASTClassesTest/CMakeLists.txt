add_executable(ast-class-test 
    ASTClassTest.cpp    
)

target_link_libraries(ast-class-test PRIVATE
    libYulAST 
    nlohmann_json::nlohmann_json
)
target_include_directories(ast-class-test PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/lib/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
