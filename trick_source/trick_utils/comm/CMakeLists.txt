
set( TRICKCOMM_SRC
  src/tc_accept
  src/tc_blockio
  src/tc_broadcast_conninfo
  src/tc_clock_init
  src/tc_clock_time
  src/tc_connect
  src/tc_dev_copy
  src/tc_disconnect
  src/tc_error
  src/tc_init
  src/tc_init_mcast_client
  src/tc_init_mcast_server
  src/tc_init_udp_client
  src/tc_init_udp_server
  src/tc_isValid
  src/tc_listen
  src/tc_multiconnect
  src/tc_pending
  src/tc_read
  src/tc_read_byteswap
  src/tc_set_blockio
  src/tc_write
  src/tc_write_byteswap
  src/trick_bswap_buffer
  src/trick_byteswap
  src/trick_error_hndlr
)

add_library( trick_comm STATIC ${TRICKCOMM_SRC})
