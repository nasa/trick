# Trick utils files that are not in their own library (top Makefile's
# UTILS_DIRS minus the dirs that build a separate archive, A3).
set( TRICK_UTILS_SRC
  compareFloatingPoint/src/compareFloatingPoint.cpp
  interpolator/src/Interpolator.cpp
  shm/src/tsm_disconnect.c
  shm/src/tsm_init.c
  shm/src/tsm_init_with_lock.c
  shm/src/tsm_reconnect.c
  trick_adt/src/MapStrToPtr.c
  trick_adt/src/bst.c
  trick_adt/src/bubble_sort.c
  trick_adt/src/dllist.c
  trick_adt/src/lqueue.c
  trick_adt/src/lstack.c
  trick_adt/src/record_array.c
  unicode/src/unicode_utils.c
)
add_library( trick_utils_objs OBJECT ${TRICK_UTILS_SRC} )
target_link_libraries( trick_utils_objs PUBLIC trick_build_flags )

###############################################################
# Other Trick libraries (each packaged into its own archive, A3)
###############################################################

add_subdirectory(comm)
add_subdirectory(connection_handlers)
add_subdirectory(math)
add_subdirectory(optimization)
add_subdirectory(units)
add_subdirectory(var_binary_parser)
