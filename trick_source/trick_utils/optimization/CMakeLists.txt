set( TRICK_OPTIMIZATION_SRC
  src/dPDIP_solver.c
)

add_library( trick_optimization STATIC ${TRICK_OPTIMIZATION_SRC})
target_link_libraries( trick_optimization PUBLIC trick_build_flags )
set_target_properties( trick_optimization PROPERTIES ARCHIVE_OUTPUT_NAME trick_optimization)
