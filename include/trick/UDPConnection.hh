#ifndef UDP_CONNECTION_HH
#define UDP_CONNECTION_HH

/*
    PURPOSE: ( Encapsulate a UDP socket. )
*/

#include "trick/ClientConnection.hh"
#include "trick/SystemInterface.hh"


namespace Trick {
    
    class UDPConnection : public ClientConnection {
        public:

            UDPConnection ();
            UDPConnection (SystemInterface * system_interface);

            int start() override;

            int write (const std::string& message) override;
            int write (char * message, int size) override;

            std::string read  (int max_len = MAX_CMD_LEN) override;

            int disconnect () override;
            bool isInitialized() override;

            int setBlockMode(bool block_mode) override;
            int restart() override;

            // Non-override functions
            int initialize(const std::string& hostname, int port);


            int getPort();
            std::string getHostname();


        private:
            bool _initialized;
            bool _started;
            int _socket;

            int _port;
            std::string _hostname;

            struct sockaddr_in _remote_serv_addr;

            SystemInterface * _system_interface;     /* ** */
    };

}

#endif