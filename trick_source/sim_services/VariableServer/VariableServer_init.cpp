
#include "trick/VariableServer.hh"
#include "trick/exec_proto.hh"

int Trick::VariableServer::init() {
    /* Start up the listen thread for the input processor variable server.
       The thread is started whenever the variable server is enabled OR broadcasting
       is on (broadcast defaults on). This lets a sim announce itself on the multicast
       broadcast channel (for tools like sim-sniffer) even while the variable server is
       disabled. Connection servicing remains gated by get_allow_connections() inside
       the listen thread, so a disabled sim broadcasts but will not accept connections. */
    if (enabled || listen_thread.get_broadcast())
    {
        int ret = listen_thread.check_and_move_listen_device() ;
        if ( ret != 0 ) {
            return ret ;
        }
        listen_thread.create_thread() ;
    }

    return(0) ;
}
