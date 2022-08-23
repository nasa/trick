
#include "trick/VariableServer.hh"
#include "trick/release.h"

void Trick::VariableServerThread::wait_for_accept() {
    // TODO - this needs to be synchronized
    while ( connection_accepted == false ) {
        RELEASE() ;
    }
}
