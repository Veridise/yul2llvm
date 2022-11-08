cmake_minimum_required(VERSION 3.18)

project(Yul2LLVM
  DESCRIPTION ""
  VERSION 0.1.0
  HOMEPAGE_URL https://github.com/Veridise/yul2llvm
)

include(GNUInstallDirs)

set(CMAKE_CXX_FLAGS_DEBUG -fsanitize=address)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE STRING "")
# Use same policy as LLVM to suppress warnings
if(POLICY CMP0116)
  cmake_policy(SET CMP0116 OLD)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

### Dependency setup
find_package(nlohmann_json 3 REQUIRED)
find_package(LLVM 13 REQUIRED CONFIG)

message(STATUS "Using LLVM in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(TableGen)
include(AddLLVM)
include(HandleLLVMOptions)

# include_directories(${LLVM_INCLUDE_DIRS})
# separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
# add_definitions(${LLVM_DEFINITIONS_LIST})
add_compile_options(-Wswitch-enum)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON CACHE BOOL "Hide inlines")
# For static LLVM, ensure hidden visibility to be consistent with LLVM build
if(NOT ${BUILD_SHARED_LIBS})
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif()

### Project files

set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc")

# include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/KSIRMacros.cmake)

add_subdirectory(lib)

# add_subdirectory(include)
# add_subdirectory(lib)
# add_subdirectory(tools)

### Documentation
add_custom_target(doc)
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if (Doxygen_FOUND)
  message(STATUS "Doxygen found, enabling documentation...")
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
  set(DOXYGEN_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/include/")
  set(DOXYGEN_EXCLUDE_PATTERNS ${CMAKE_CURRENT_BINARY_DIR}/include/*/*.md)
  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/index.md")
  set(DOXYGEN_FILE_PATTERNS *.cpp *.cpp.inc *.h.inc *.hpp *.h *.td *.md *.py)
  set(DOXYGEN_EXTENSION_MAPPING inc=C++)
  set(DOXYGEN_SOURCE_BROWSER YES)
  set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
  doxygen_add_docs(doxygen
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/"
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/doxygen/"
  )
  # add_dependencies(doxygen KSIRDialectHeaders mlir-doc)
  add_dependencies(doc doxygen)
endif()

### Tests

# testing
find_package(GTest)
include(CTest)
enable_testing()
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Catch-all target for running all tests
add_custom_target(check
  DEPENDS
  # test targets may not exist if BUILD_TESTING is off
  $<$<TARGET_EXISTS:check-lit>:check-lit>
  $<$<TARGET_EXISTS:check-unit>:check-unit>)

### Install

export(EXPORT Yul2LLVMTargets
  FILE ${CMAKE_CURRENT_BINARY_DIR}/Yul2LLVMTargets.cmake
  NAMESPACE Yul2LLVM::
)
install(EXPORT Yul2LLVMTargets
  NAMESPACE Yul2LLVM::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)
