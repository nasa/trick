# FindUDUNITS2
# ------------
#
# Finds the UDUNITS-2 units library. UDUNITS-2 installs neither a CMake package
# nor a pkg-config file, so this module searches for the header and library.
#
# Some distributions put the header in include/udunits2/, others directly in
# include/. Set UDUNITS2_ROOT to search a specific installation first.
#
# Imported target:
#   UDUNITS2::UDUNITS2
#
# Result variables:
#   UDUNITS2_FOUND
#   UDUNITS2_INCLUDE_DIRS
#   UDUNITS2_LIBRARIES

find_path(UDUNITS2_INCLUDE_DIR
    NAMES udunits2.h
    PATH_SUFFIXES udunits2
    DOC "Directory containing udunits2.h")
find_library(UDUNITS2_LIBRARY
    NAMES udunits2
    DOC "UDUNITS-2 library")
mark_as_advanced(UDUNITS2_INCLUDE_DIR UDUNITS2_LIBRARY)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UDUNITS2
    REQUIRED_VARS UDUNITS2_LIBRARY UDUNITS2_INCLUDE_DIR)

if(UDUNITS2_FOUND)
    set(UDUNITS2_INCLUDE_DIRS "${UDUNITS2_INCLUDE_DIR}")
    set(UDUNITS2_LIBRARIES "${UDUNITS2_LIBRARY}")
    if(NOT TARGET UDUNITS2::UDUNITS2)
        add_library(UDUNITS2::UDUNITS2 UNKNOWN IMPORTED)
        set_target_properties(UDUNITS2::UDUNITS2 PROPERTIES
            IMPORTED_LOCATION "${UDUNITS2_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${UDUNITS2_INCLUDE_DIR}")
    endif()
endif()
