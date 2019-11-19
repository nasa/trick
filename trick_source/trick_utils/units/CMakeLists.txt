
set( TRICK_UNITS_SRC
  src/UCFn.cpp
  src/Unit.cpp
  src/units_conv
)

add_library( trick_units STATIC ${TRICK_UNITS_SRC})

