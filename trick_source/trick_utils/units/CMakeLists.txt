set( TRICK_UNITS_SRC
  src/UCFn.cpp
  src/Unit.cpp
  src/units_conv.c
)

add_library( trick_units STATIC ${TRICK_UNITS_SRC})
target_link_libraries( trick_units PUBLIC trick_build_flags )
set_target_properties( trick_units PROPERTIES ARCHIVE_OUTPUT_NAME trick_units)
