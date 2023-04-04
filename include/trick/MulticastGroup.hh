/*
    PURPOSE: ( Encapsulate multicast functionality. )
*/
#ifndef MULTICAST_GROUP_HH
#define MULTICAST_GROUP_HH

#include <string>
#include <vector>
#include <arpa/inet.h>

#include <trick/UDPConnection.hh>
#include <trick/SystemInterface.hh>

namespace Trick {
    class MulticastGroup : public UDPConnection {
        public:
            MulticastGroup();
            MulticastGroup (SystemInterface * system_interface);

            virtual ~MulticastGroup();
            
            // Multicast specific functions
            int initialize_with_receiving(std::string local_addr, std::string mcast_addr, int port);
            virtual int initialize();

            virtual int broadcast (std::string message);
            virtual int addAddress (std::string addr, int port);

            // ClientConnection interface

            virtual int write (const std::string& message) override;
            virtual int write (char * message, int size) override;

            virtual int read  (std::string& message, int max_len = MAX_CMD_LEN) override;

            virtual int disconnect () override;

            virtual bool isInitialized() override;

            virtual int restart() override;

            virtual std::string getClientHostname() override;
            virtual int getClientPort() override;
            
        private:
            std::vector<struct sockaddr_in> _addresses;           /**<  trick_io(**) Addresses to multicast to. */
            bool _initialized;                                    /**<  trick_io(**) Whether this object is ready */

            struct sockaddr_in _self_info ;                       /**<  trick_io(**) Save self information so we don't process our own messages */

            SystemInterface * _system_interface;
    };
}

#endif