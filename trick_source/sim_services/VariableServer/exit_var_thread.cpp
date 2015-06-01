
#include "trick/VariableServer.hh"
#include "trick/tc_proto.h"

void exit_var_thread(void *in_vst) {
    Trick::VariableServerThread * vst = (Trick::VariableServerThread *) in_vst ;
    Trick::VariableServer * vs = vst->get_vs() ;

    tc_disconnect(&vst->get_connection());

    // Tell the variable server that this thread is exiting.
    vs->delete_vst(vst->get_pthread_id()) ;

    // This will deleting the vst object from within the object itself. exit_var_thread
    // is called from within Trick::VariableServerThread::thread_body.
    // I am claiming this is safe as this is the last thing routing is the last thing
    // that touches vst. The C++ FAQ says it's safe if you can say this. (Alex 6/9/14)
    delete vst ;
}

