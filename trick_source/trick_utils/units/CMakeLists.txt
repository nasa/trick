# libtrick_units: Trick's original units conversion, used by data products and ICG.
trick_add_library(trick_units STATIC
    EXPORT_NAME units
    SOURCES
        src/UCFn.cpp
        src/Unit.cpp
        src/units_conv.c
)
