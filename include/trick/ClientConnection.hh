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
            // Should this be here? ¯\_(ツ)_/¯
            enum ConnectionType { TCP, UDP, MCAST, WS } ;

            virtual int initialize() = 0;

            virtual int write (const std::string& message) = 0;
            virtual int write (char * message, int size) = 0;

            virtual std::string read  (int max_len = MAX_CMD_LEN) = 0;

            virtual int disconnect () = 0;
            virtual std::string get_client_tag () = 0;
            virtual int set_client_tag(std::string tag) = 0;
            virtual int setBlockMode (int mode) = 0;
            
            static const unsigned int MAX_CMD_LEN = 200000 ;

        protected:
            ConnectionType _connection_type;

            // RHEL appears to have an issue with std::atomic
            // std::atomic_bool _is_initialized;

    };
}

#endif