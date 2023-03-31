/*
    PURPOSE: ( Encapsulate multicast functionality. )
*/
#ifndef MULTICAST_GROUP_HH
#define MULTICAST_GROUP_HH

#include <string>
#include <vector>
#include <arpa/inet.h>

#include <trick/SystemInterface.hh>

namespace Trick {
    class MulticastGroup {
        public:
            MulticastGroup();
            MulticastGroup (SystemInterface * system_interface);

            virtual ~MulticastGroup();
            
            virtual int broadcast (std::string message);
            virtual int addAddress (std::string addr, int port);
            virtual int restart ();
            virtual int initialize();
            virtual int isInitialized();
            virtual int disconnect();


        private:
            std::vector<struct sockaddr_in> _addresses;           /**<  trick_io(**) Addresses to multicast to. */
            int _mcast_socket;                                    /**<  trick_io(**) Socket opened in initialization. */
            bool _initialized;                                     /**<  trick_io(**) Whether manager is ready */

            SystemInterface * _system_interface;
    };
}

#endif