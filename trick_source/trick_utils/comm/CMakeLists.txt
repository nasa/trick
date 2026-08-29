set( TRICKCOMM_SRC
  src/tc_accept.c
  src/tc_blockio.c
  src/tc_broadcast_conninfo.c
  src/tc_clock_init.c
  src/tc_clock_time.c
  src/tc_connect.c
  src/tc_dev_copy.c
  src/tc_disconnect.c
  src/tc_error.c
  src/tc_init.c
  src/tc_init_mcast_client.c
  src/tc_init_mcast_server.c
  src/tc_init_udp_client.c
  src/tc_init_udp_server.c
  src/tc_isValid.c
  src/tc_listen.c
  src/tc_multiconnect.c
  src/tc_pending.c
  src/tc_read.c
  src/tc_read_byteswap.c
  src/tc_set_blockio.c
  src/tc_write.c
  src/tc_write_byteswap.c
  src/trick_bswap_buffer.c
  src/trick_bswap_single_parameter.c
  src/trick_byteswap.c
  src/trick_error_hndlr.c
)

add_library( trick_comm STATIC ${TRICKCOMM_SRC})
target_link_libraries( trick_comm PUBLIC trick_build_flags )
set_target_properties( trick_comm PROPERTIES ARCHIVE_OUTPUT_NAME trick_comm)
