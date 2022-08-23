#ifndef CLIENT_LISTENER_HH
#define CLIENT_LISTENER_HH

#include "trick/tc.h"
#include "trick/TCConnection.hh"
#include <string>

namespace Trick {
    
    class TCConnection;

    class ClientListener {
        public:
            ClientListener ();
            ~ClientListener ();

            // We'll see if we need separate methods for these
            int initialize(std::string hostname, int port);
            int initialize();

            int setBlockMode(TCCommBlocking mode);

            bool checkForNewConnections();

            const char * getHostname ();

            int getPort();

            int disconnect();

            int checkSocket();

            bool validateSourceAddress(std::string source_address);

            bool isInitialized(); 

            friend int accept(ClientListener* listener, TCConnection* connection);

        private:
            TCDevice _listen_dev;
            std::string saved_source;
            int port;
            bool initialized;
    };
}

#endif