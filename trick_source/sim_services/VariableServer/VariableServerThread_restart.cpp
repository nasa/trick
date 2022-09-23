
#include <stdlib.h>
#include "trick/VariableServerThread.hh"

void Trick::VariableServerThread::preload_checkpoint() {

    // Stop variable server processing at the top of the processing loop.
    pthread_mutex_lock(&restart_pause);

    // Let the thread complete any data copying it has to do
    // and then suspend data copying until the checkpoint is reloaded.
    pthread_mutex_lock(&copy_mutex);

    // Save the pause state of this thread.
    saved_pause_cmd = pause_cmd;

    // Disallow data writing.
    pause_cmd = true ;

    // Temporarily "disconnect" the variable references from Trick Managed Memory
    // by tagging each as a "bad reference".
    std::vector <VariableReference *>::iterator it ;
    for (it = vars.begin(); it != vars.end() ; it++) {
        (*it)->ref->address = (char*)&bad_ref_int;
        (*it)->ref->attr = new ATTRIBUTES() ;
        (*it)->ref->attr->type = TRICK_NUMBER_OF_TYPES ;
        (*it)->ref->attr->units = (char *)"--" ;
        (*it)->ref->attr->size = sizeof(int) ;
    }

    // Allow data copying to continue.
    pthread_mutex_unlock(&copy_mutex);

}

void Trick::VariableServerThread::restart() {
    // Set the pause state of this thread back to its "pre-checkpoint reload" state.
    pause_cmd = saved_pause_cmd ;

    // Restart the variable server processing.
    pthread_mutex_unlock(&restart_pause);

}


