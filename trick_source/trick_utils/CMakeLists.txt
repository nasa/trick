

# Trick utils files that are not in their own library
set( TRICK_UTILS_SRC
  interpolator/src/Interpolator.cpp
  shm/src/tsm_disconnect
  shm/src/tsm_init
  shm/src/tsm_init_with_lock
  shm/src/tsm_reconnect
  trick_adt/src/MapStrToPtr
  trick_adt/src/bst
  trick_adt/src/bubble_sort
  trick_adt/src/dllist
  trick_adt/src/lqueue
  trick_adt/src/lstack
  trick_adt/src/record_array
  unicode/src/unicode_utils
)
add_library( trick_utils_objs OBJECT ${TRICK_UTILS_SRC} )

###############################################################
# Other Trick libraries
###############################################################

add_subdirectory(comm)
add_subdirectory(math)
add_subdirectory(units)

