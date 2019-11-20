
add_subdirectory(Var)
add_subdirectory(Log)
add_subdirectory(EQParse)
add_subdirectory(units)

if( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fermi-ware/CMakeLists.txt )
    add_subdirectory(fermi-ware)
endif()

add_subdirectory(DPX)
