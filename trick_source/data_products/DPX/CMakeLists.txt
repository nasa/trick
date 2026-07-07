# ported from this directory's own makefile: LIBDIRS always build; APPDIRS
# (GXPLOT, and FXPLOT if fermi-ware/ exists) only when USE_X_WINDOWS.
add_subdirectory(DPM)
add_subdirectory(DPC)
add_subdirectory(DPV/UTILS)

if(USE_X_WINDOWS)
    add_subdirectory(APPS/GXPLOT)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../fermi-ware/CMakeLists.txt)
        add_subdirectory(APPS/FXPLOT)
    endif()
endif()
