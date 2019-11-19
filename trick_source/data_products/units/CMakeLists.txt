
set ( DP_UNITS_SRC
  init_units_system
  map_trick_units_to_udunits
  units_conv
)

add_library( dp_units STATIC ${DP_UNITS_SRC})
target_include_directories( dp_units PUBLIC ${UDUNITS2_INCLUDES} )

