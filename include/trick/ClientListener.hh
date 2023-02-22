#ifndef CLIENT_LISTENER_HH
#define CLIENT_LISTENER_HH

#include "trick/tc.h"
#include "trick/TCConnection.hh"
#include <string>

namespace Trick {
    
    class TCConnection;

    class ClientListener {
        public:
            friend class TCConnection;

            ClientListener ();
            ~ClientListener ();

            // We'll see if we need separate methods for these
            int initialize(std::string hostname, int port);
            int initialize();

            int set_block_mode(TCCommBlocking mode);

            bool checkForNewConnections();

            const char * getHostname ();

            int getPort();

            int disconnect();

            int checkSocket();

            bool validateSourceAddress(std::string source_address);

            bool isInitialized(); 
            
        private:
            TCDevice _listen_dev;
            std::string saved_source;
            int port;
            bool initialized;
    };
}

#endif