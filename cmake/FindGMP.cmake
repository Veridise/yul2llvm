include(FindPackageHandleStandardArgs)

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY NAMES gmp)

find_package_handle_standard_args(GMP
  FOUND_VAR GMP_FOUND
  REQUIRED_VARS
    GMP_INCLUDE_DIR
    GMP_LIBRARY
  VERSION_VAR GMP_VERSION
)

if(GMP_FOUND AND NOT TARGET GMP::GMP)
  add_library(GMP::GMP UNKNOWN IMPORTED)
  set_target_properties(GMP::GMP PROPERTIES
    IMPORTED_LOCATION "${GMP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}"
  )
endif()

mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY)
