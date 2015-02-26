/*
 * $Id: VariableServerThread.cpp 432 2009-12-07 22:10:08Z hchen $
 */

#include "sim_services/VariableServer/include/VariableServer.hh"
#include "sim_services/include/release.h"

void Trick::VariableServerThread::wait_for_accept() {
    while ( connection_accepted == false ) {
        RELEASE() ;
    }
}
