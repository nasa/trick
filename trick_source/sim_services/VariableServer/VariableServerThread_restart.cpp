
#include <stdlib.h>
#include "trick/VariableServerThread.hh"

// This should maybe go somewhere completely different
// Leaving it in thread for now
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
    session->disconnect_references();


    // Allow data copying to continue.
    pthread_mutex_unlock(&copy_mutex);

}

void Trick::VariableServerThread::restart() {
    // Set the pause state of this thread back to its "pre-checkpoint reload" state.
    pause_cmd = saved_pause_cmd ;

    // Restart the variable server processing.
    pthread_mutex_unlock(&restart_pause);

}


