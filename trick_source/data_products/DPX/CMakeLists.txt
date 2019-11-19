
add_subdirectory(DPM)
add_subdirectory(DPC)
add_subdirectory(DPV/UTILS)
add_subdirectory(APPS/GXPLOT)
if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../fermi-ware/CMakeLists.txt )
    add_subdirectory(APPS/FXPLOT)
endif()
