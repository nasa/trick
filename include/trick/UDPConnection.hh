#ifndef UDP_CONNECTION_HH
#define UDP_CONNECTION_HH

/*
    PURPOSE: ( Encapsulate a UDP connection with TrickComm. )
*/

#include "trick/ClientConnection.hh"
#include "trick/ClientListener.hh"
#include "tc.h"


namespace Trick {
    
    class ClientListener;

    class UDPConnection : public ClientConnection {
        public:

            UDPConnection ();

            int initialize() override;
            int establish_connection() override;

            int write (const std::string& message) override;
            int write (char * message, int size) override;

            std::string read  (int max_len) override;

            int disconnect () override;
            std::string get_client_tag () override;
            int set_client_tag(std::string tag) override;
            int get_socket();

            int set_block_mode(int block_mode) override;

            // Non-override functions
            int set_error_reporting (bool on);
            int initialize_udp(const std::string& hostname, int port);


        private:
            TCDevice _device;   /**<  trick_io(**) */
            bool _connected;
    };

}

#endif