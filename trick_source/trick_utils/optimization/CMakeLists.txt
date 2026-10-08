# libtrick_optimization: primal-dual interior point solver.
trick_add_library(trick_optimization STATIC
    EXPORT_NAME optimization
    SOURCES
        src/dPDIP_solver.c
)

target_link_libraries(trick_optimization PRIVATE trick_math m)
