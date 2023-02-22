
#include "trick/VariableServer.hh"

void exit_var_thread(void *in_vst) {
    Trick::VariableServerThread * vst = (Trick::VariableServerThread *) in_vst ;
    Trick::VariableServer * vs = vst->get_vs() ;

    if (vst->get_pthread_id() != pthread_self()) {
        std::cerr << "exit_var_thread must be called from the variable server thread" << std::endl;
    }
    
    vs->delete_session(vst->get_pthread_id());

    // Tell the variable server that this thread is exiting.
    vs->delete_vst(vst->get_pthread_id()) ;

    vst->cleanup();
}

