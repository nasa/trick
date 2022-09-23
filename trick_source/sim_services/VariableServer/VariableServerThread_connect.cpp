
#include "trick/VariableServer.hh"
#include "trick/release.h"

void Trick::VariableServerThread::wait_for_accept() {
    while ( connection_accepted == false ) {
        RELEASE() ;
    }
}
