# trick-trk2ascii — ported from this directory's own makefile (the
# authority). No stale 2019 CMakeLists.txt existed for this directory.
#
# The makefile's link line is `-L../../lib_<cpu> -llog -lvar -L$(TRICK_LIB_DIR)
# -ltrick_units`: -L search paths are cumulative in link order, so
# `-ltrick_units` actually resolves against data_products/units's own
# libtrick_units.a (the first -L given) rather than trick_utils/units'
# same-named archive at TRICK_LIB_DIR — confirmed by nm against a real
# make build's liblog.a/libtrick_units.a in trick_source/data_products/
# lib_Darwin_25 (TrickBinary.cpp calls map_trick_units_to_udunits(), which
# only data_products/units/map_trick_units_to_udunits.cpp defines; the core
# trick_utils/units archive does not). Link dp_units, not trick_units.
add_executable(trick-trk2ascii trk2ascii.cpp)
target_include_directories(trick-trk2ascii PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(trick-trk2ascii PRIVATE dp_log dp_var dp_units)
if(APPLE)
    target_link_libraries(trick-trk2ascii PRIVATE -lc++abi)
endif()

install(TARGETS trick-trk2ascii RUNTIME DESTINATION bin)
