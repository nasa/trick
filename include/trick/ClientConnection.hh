/************************************************************************
PURPOSE: (Abstract base class for a connection to a client. Should
be inherited by variable server and web server connections. )
LIBRARY DEPENDENCIES:
    () )
**************************************************************************/

#ifndef CLIENT_CONNECTION_HH
#define CLIENT_CONNECTION_HH

#include <string>
// #include <atomic>

namespace Trick {
    class ClientConnection {
        public: 
            static const unsigned int MAX_CMD_LEN = 200000 ;

            enum ConnectionType { TCP, UDP, MCAST, WS } ;

            // Pure virtual methods
            virtual int initialize() = 0;
            virtual int establish_connection() = 0;

            virtual int write (const std::string& message) = 0;
            virtual int write (char * message, int size) = 0;

            virtual std::string read  (int max_len = MAX_CMD_LEN) = 0;

            virtual int disconnect () = 0;
            virtual std::string get_client_tag () = 0;
            virtual int set_client_tag(std::string tag) = 0;
            virtual int set_block_mode (int mode) = 0;
            virtual int get_socket () = 0;

        protected:
            ConnectionType _connection_type;

    };
}

#endif