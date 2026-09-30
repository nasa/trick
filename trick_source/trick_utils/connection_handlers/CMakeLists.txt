# libtrick_connection_handlers: connection classes used by the variable server.
trick_add_library(trick_connection_handlers STATIC
    EXPORT_NAME connection_handlers
    SOURCES
        ClientConnection.cpp
        MulticastGroup.cpp
        TCPClientListener.cpp
        TCPConnection.cpp
        UDPConnection.cpp
)
