#ifndef TC_CONNECTION_HH
#define TC_CONNECTION_HH

#include "trick/ClientConnection.hh"
#include "trick/ClientListener.hh"
#include "tc.h"


namespace Trick {
    
    class ClientListener;

    class TCConnection : public ClientConnection {
        public:

            TCConnection ();

            int initialize() override;
            int write (std::string& message, int len) override;
            int read  (std::string& message, int len) override;
            int disconnect () override;
            std::string get_client_tag () override;
            int set_client_tag(std::string tag) override;

            int setBlockMode(int block_mode) override;
            int setErrorReporting (bool on);

            static const unsigned int MAX_CMD_LEN = 200000 ;

            friend int accept(ClientListener* listener, TCConnection* connection);

        private:
            TCDevice _device;
    };

    int accept(ClientListener* listener, TCConnection* connection);
}

#endif