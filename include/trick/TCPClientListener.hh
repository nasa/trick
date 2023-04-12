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

    class TCPClientListener {
        public:

            TCPClientListener ();
            TCPClientListener (SystemInterface * system_interface);

            virtual ~TCPClientListener ();

            virtual int initialize(std::string hostname, int port);
            virtual int initialize();

            virtual int setBlockMode(bool blocking);

            virtual bool checkForNewConnections();

            virtual TCPConnection * setUpNewConnection ();

            virtual int disconnect();

            virtual int checkSocket();

            virtual bool validateSourceAddress(std::string source_address);

            virtual bool isInitialized(); 
            
            virtual int restart();

            virtual std::string getHostname ();
            virtual int getPort();
            
        protected:
        
            int _listen_socket;
            std::string _hostname;
            int _port;
            std::string _client_tag;

            bool _initialized;

            SystemInterface * _system_interface;    /* ** */
    };
}

#endif