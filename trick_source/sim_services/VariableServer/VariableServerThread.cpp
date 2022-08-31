
#include <iostream>
#include <stdlib.h>
#include "trick/VariableServerThread.hh"
#include "trick/exec_proto.h"
#include "trick/TrickConstant.hh"

Trick::VariableServer * Trick::VariableServerThread::vs = NULL ;

Trick::VariableServerThread::VariableServerThread(TCDevice * in_listen_dev) :
 Trick::ThreadBase("VarServer") ,
 listen_dev(in_listen_dev), session(NULL) {

    std::cout << "Creating VST" << std::endl;

    connection_accepted = false ;

    connection.initialize();

    pthread_mutex_init(&connection_accepted_mutex, NULL);
    pthread_cond_init(&connection_accepted_cv, NULL);

    pthread_mutex_init(&restart_pause, NULL);

}

Trick::VariableServerThread::~VariableServerThread() {}


std::ostream& Trick::operator<< (std::ostream& s, Trick::VariableServerThread& vst) {
    // TODO: Replicate this functionality


    // Write a JSON representation of a Trick::VariableServerThread to an ostream.
    // struct sockaddr_in otherside;
    // socklen_t len = (socklen_t)sizeof(otherside);

    // s << "  \"connection\":{\n";
    // s << "    \"client_tag\":\"" << vst.connection.client_tag << "\",\n";

    // int err = getpeername(vst.connection.socket, (struct sockaddr*)&otherside, &len);

    // if (err == 0) {
    //     s << "    \"client_IP_address\":\"" << inet_ntoa(otherside.sin_addr) << "\",\n";
    //     s << "    \"client_port\":\"" << ntohs(otherside.sin_port) << "\",\n";
    // } else {
    //     s << "    \"client_IP_address\":\"unknown\",\n";
    //     s << "    \"client_port\":\"unknown\",";
    // }

    // if (vst.binary_data) {
    //     s << "    \"format\":\"BINARY\",\n";
    // } else {
    //     s << "    \"format\":\"ASCII\",\n";
    // }
    // s << "    \"update_rate\":" << vst.update_rate << ",\n";

    // s << "    \"variables\":[\n";

    // int n_vars = (int)vst.vars.size();
    // for (int i=0 ; i<n_vars ; i++) {
    //     s << *(vst.vars[i]);
    //     if ((n_vars-i)>1) {
    //         s << "," ;
    //     }
    //     s << "\n";
    // }
    // s << "    ]\n";
    // s << "  }" << std::endl;
    // return s;
}

void Trick::VariableServerThread::set_vs_ptr(Trick::VariableServer * in_vs) {
    vs = in_vs ;
}

Trick::VariableServer * Trick::VariableServerThread::get_vs() {
    return vs ;
}

void Trick::VariableServerThread::set_client_tag(std::string tag) {
    connection.set_client_tag(tag);
}



// TCDevice & Trick::VariableServerThread::get_connection() {
//     return connection ;
// }

void Trick::VariableServerThread::wait_for_accept() {

    pthread_mutex_lock(&connection_accepted_mutex);
    while ( connection_accepted == false ) {
        pthread_cond_wait(&connection_accepted_cv, &connection_accepted_mutex);
    }
    pthread_mutex_unlock(&connection_accepted_mutex);

}

// This should maybe go somewhere completely different
// Leaving it in thread for now
void Trick::VariableServerThread::preload_checkpoint() {

    // Stop variable server processing at the top of the processing loop.
    pthread_mutex_lock(&restart_pause);

    // Let the thread complete any data copying it has to do
    // and then suspend data copying until the checkpoint is reloaded.
    pthread_mutex_lock(&(session->copy_mutex));

    // Save the pause state of this thread.
    saved_pause_cmd = session->get_pause();

    // Disallow data writing.
    session->set_pause(true);

    // Temporarily "disconnect" the variable references from Trick Managed Memory
    // by tagging each as a "bad reference".
    session->disconnect_references();


    // Allow data copying to continue.
    pthread_mutex_unlock(&(session->copy_mutex));

}

void Trick::VariableServerThread::restart() {
    // Set the pause state of this thread back to its "pre-checkpoint reload" state.
    session->set_pause(saved_pause_cmd) ;

    // Restart the variable server processing.
    pthread_mutex_unlock(&restart_pause);

}


