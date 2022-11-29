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
            std::vector<struct sockaddr_in> addresses;
            int mcast_socket;
            int initialized;

    };
}