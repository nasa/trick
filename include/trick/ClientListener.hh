#ifndef CLIENT_LISTENER_HH
#define CLIENT_LISTENER_HH

/*
    PURPOSE: ( Encapsulate a TCP server. )
*/

#include <string>
#include "trick/SystemInterface.hh"
#include "trick/TCPConnection.hh"

#define LISTENER_ERROR -1


namespace Trick {
    
    class TCPConnection;

    class ClientListener {
        public:

            ClientListener ();
            ClientListener (SystemInterface * system_interface);

            ~ClientListener ();

            int initialize(std::string hostname, int port);
            int initialize();

            int setBlockMode(bool blocking);

            bool checkForNewConnections();

            TCPConnection * setUpNewConnection ();

            std::string getHostname ();

            int getPort();

            int disconnect();

            int checkSocket();

            bool validateSourceAddress(std::string source_address);

            bool isInitialized(); 

            int restart();
            
        private:
        
            int _listen_socket;
            std::string _hostname;
            int _port;
            std::string _client_tag;

            bool _initialized;

            SystemInterface * _system_interface;    /* ** */
    };
}

#endif