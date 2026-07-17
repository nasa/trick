set( TRICK_CONNECTION_HANDLERS_SRC
  ClientConnection.cpp
  MulticastGroup.cpp
  TCPClientListener.cpp
  TCPConnection.cpp
  UDPConnection.cpp
)

add_library( trick_connection_handlers STATIC ${TRICK_CONNECTION_HANDLERS_SRC})
target_link_libraries( trick_connection_handlers PUBLIC trick_build_flags )
set_target_properties( trick_connection_handlers PROPERTIES ARCHIVE_OUTPUT_NAME trick_connection_handlers)
