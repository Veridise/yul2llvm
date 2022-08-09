add_library(libYulAST 
  YulASTBase.cpp
  YulAssignmentNode.cpp 
  )
target_include_directories(libYulAST PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_link_libraries(libYulAST PUBLIC
  nlohmann_json::nlohmann_json
)
