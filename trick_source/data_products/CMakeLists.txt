# data_products: ported from this directory's own makefile (the authority).
# LIBDIRS always build; fermi-ware only exists on some checkouts (this one
# doesn't have it — matches DPX's FXPLOT gate below); APPDIRS = DPX,
# Apps/Trk2csv, Apps/ExternalPrograms.
add_subdirectory(Var)
add_subdirectory(Log)
add_subdirectory(EQParse)
add_subdirectory(units)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fermi-ware/CMakeLists.txt)
    add_subdirectory(fermi-ware)
endif()

add_subdirectory(DPX)
add_subdirectory(Apps/Trk2csv)
add_subdirectory(Apps/ExternalPrograms)

# Umbrella target for the Phase 3 `stage` install target to depend on.
add_custom_target(trick-data-products)
add_dependencies(trick-data-products
    dp_var dp_log dp_eqparse dp_units
    DPM DPC dpv_utils
    trick-trk2ascii
    dp_test dp_subtract
)
if(USE_X_WINDOWS)
    add_dependencies(trick-data-products trick-gxplot)
endif()
