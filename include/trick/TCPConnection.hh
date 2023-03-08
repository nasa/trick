#ifndef TCP_CONNECTION_HH
#define TCP_CONNECTION_HH

/*
    PURPOSE: ( Encapsulate a TCP connection. )
*/

#include "trick/ClientConnection.hh"
#include "trick/SystemInterface.hh"

namespace Trick {
    
    class TCPConnection : public ClientConnection {
        public:

            TCPConnection (int listen_socket);
            TCPConnection (SystemInterface * system_interface);
            TCPConnection (int listen_socket, SystemInterface * system_interface);

            int start() override;

            int write (const std::string& message) override;
            int write (char * message, int size) override;

            std::string read  (int max_len = MAX_CMD_LEN) override;

            int disconnect () override;
            bool isInitialized() override;

            int setBlockMode(bool blocking) override;

            int restart() override;

        private:
            int _socket;
            bool _connected;

            // This is needed to be able to accept an incoming connection, after being set up by the listener
            int _listen_socket;

            SystemInterface * _system_interface;     /* ** */

    };
}

#endif