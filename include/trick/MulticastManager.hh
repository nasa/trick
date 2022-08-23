/*
    PURPOSE: ( Encapsulate multicast functionality. )
*/

#include <string>
#include <vector>
#include <arpa/inet.h>

namespace Trick {
    class MulticastManager {
        public:
            MulticastManager();
            ~MulticastManager();
            
            int broadcast (std::string message);
            int addAddress (std::string addr, int port);
            int restart ();
            int initialize();
            int is_initialized();


        private:
            std::vector<struct sockaddr_in> addresses;           /**<  trick_io(**) Addresses to multicast to. */
            int mcast_socket;                                    /**<  trick_io(**) Socket opened in initialization. */
            int initialized;                                     /**<  trick_io(**) Whether manager is ready */

    };
}