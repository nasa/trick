/*
 * $Id: VariableServer_listen.cpp 1884 2011-08-25 18:36:19Z lin $
 */

#include "sim_services/VariableServer/include/VariableServer.hh"

int Trick::VariableServer::default_data() {
    return listen_thread.init_listen_device() ;
}

