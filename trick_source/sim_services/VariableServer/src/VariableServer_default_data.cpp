
#include "sim_services/VariableServer/include/VariableServer.hh"

int Trick::VariableServer::default_data() {
    return listen_thread.init_listen_device() ;
}

