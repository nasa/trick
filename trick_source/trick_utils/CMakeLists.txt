# Libraries built as their own archives
add_subdirectory(comm)
add_subdirectory(connection_handlers)
add_subdirectory(math)
add_subdirectory(optimization)
add_subdirectory(units)
add_subdirectory(var_binary_parser)

# Object libraries compiled into libtrick.a
add_subdirectory(compareFloatingPoint)
add_subdirectory(interpolator)
add_subdirectory(shm)
add_subdirectory(trick_adt)
add_subdirectory(unicode)
