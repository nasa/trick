# libtrick_units.a (data_products' own copy — unrelated to and not linked
# against trick_source/trick_utils/units' same-named archive; see this
# directory's makefile, whose output nothing else in the make flow
# references). Built anyway to match `make all`'s artifact set.
add_library(dp_units STATIC
    init_units_system.cpp
    map_trick_units_to_udunits.cpp
    units_conv.c
)
target_include_directories(dp_units PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(dp_units PRIVATE Trick::udunits2)
set_target_properties(dp_units PROPERTIES ARCHIVE_OUTPUT_NAME trick_units)
