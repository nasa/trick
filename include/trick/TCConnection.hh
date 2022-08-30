#ifndef TC_CONNECTION_HH
#define TC_CONNECTION_HH

#include "ClientConnection.hh"
#include "tc.h"


namespace Trick {
    class TCConnection : public ClientConnection {
        public:

            TCConnection ();

            int initialize() override;
            int write (std::string& message, int len) override;
            int read  (std::string& message, int len) override;
            int disconnect () override;
            std::string get_client_tag () override;
            int set_client_tag(std::string tag) override;

            int setBlockMode(TCCommBlocking block_mode);
            int setErrorReporting (bool on);

            static const unsigned int MAX_CMD_LEN = 200000 ;

            // Maybe make a separate wrapper for the listener?
            // I think this should be called only by the listener
            // int accept (TCConnection * client);

            friend int accept(TCDevice *listener, TCConnection* connection);


        private:
            TCDevice _device;
    };
}

#endif