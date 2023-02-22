
#include <iostream>
#include <stdlib.h>
#include "trick/VariableServerThread.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/TrickConstant.hh"
#include "trick/UDPConnection.hh"
#include "trick/TCPConnection.hh"


Trick::VariableServer * Trick::VariableServerThread::vs = NULL ;

static int instance_num = 0;

Trick::VariableServerThread::VariableServerThread() :
 Trick::SysThread(std::string("VarServer" + std::to_string(instance_num++))) , debug(0), session(NULL), connection(NULL) {

    connection_status = CONNECTION_PENDING ;


    pthread_mutex_init(&connection_status_mutex, NULL);
    pthread_cond_init(&connection_status_cv, NULL);

    pthread_mutex_init(&restart_pause, NULL);

    cancellable = false;
}

Trick::VariableServerThread::~VariableServerThread() {}

std::ostream& Trick::operator<< (std::ostream& s, Trick::VariableServerThread& vst) {
    // Write a JSON representation of a Trick::VariableServerThread to an ostream.
    struct sockaddr_in otherside;
    socklen_t len = (socklen_t)sizeof(otherside);

    s << "  \"connection\":{\n";
    s << "    \"client_tag\":\"" << vst.connection->getClientTag() << "\",\n";

    // int err = getpeername(vst.connection->get_socket(), (struct sockaddr*)&otherside, &len);

    // if (err == 0) {
    //     s << "    \"client_IP_address\":\"" << inet_ntoa(otherside.sin_addr) << "\",\n";
    //     s << "    \"client_port\":\"" << ntohs(otherside.sin_port) << "\",\n";
    // } else {
    //     s << "    \"client_IP_address\":\"unknown\",\n";
    //     s << "    \"client_port\":\"unknown\",";
    // }

    pthread_mutex_lock(&vst.connection_status_mutex);
    if (vst.connection_status == CONNECTION_SUCCESS) {
        s << *(vst.session);
    }
    pthread_mutex_unlock(&vst.connection_status_mutex);

    s << "  }" << std::endl;
    return s;
}

void Trick::VariableServerThread::set_vs_ptr(Trick::VariableServer * in_vs) {
    vs = in_vs ;
}

Trick::VariableServer * Trick::VariableServerThread::get_vs() {
    return vs ;
}

void Trick::VariableServerThread::set_client_tag(std::string tag) {
    connection->setClientTag(tag);
}

int Trick::VariableServerThread::open_udp_socket(const std::string& hostname, int port) {
    UDPConnection * udp_conn = new UDPConnection();
    int status = udp_conn->initialize(hostname, port);

    connection = udp_conn;

    if (status == 0) {
        message_publish(MSG_INFO, "Created UDP variable server %s: %d\n", udp_conn->getHostname().c_str(), udp_conn->getPort());
    }

    return status;
}

int Trick::VariableServerThread::open_tcp_connection(ClientListener * listener) {
    connection = listener->setUpNewConnection();

    return 0;
}

Trick::ConnectionStatus Trick::VariableServerThread::wait_for_accept() {

    pthread_mutex_lock(&connection_status_mutex);
    while ( connection_status == CONNECTION_PENDING ) {
        pthread_cond_wait(&connection_status_cv, &connection_status_mutex);
    }
    pthread_mutex_unlock(&connection_status_mutex);

    return connection_status;
}

// This should maybe go somewhere completely different
// Leaving it in thread for now
// Gets called from the main thread as a job
void Trick::VariableServerThread::preload_checkpoint() {

    // Stop variable server processing at the top of the processing loop.
    pthread_mutex_lock(&restart_pause);

    // Make sure that the session has been initialized
    pthread_mutex_lock(&connection_status_mutex);
    if (connection_status == CONNECTION_SUCCESS) {

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
    pthread_mutex_unlock(&connection_status_mutex);
}

// Gets called from the main thread as a job
void Trick::VariableServerThread::restart() {
    // Set the pause state of this thread back to its "pre-checkpoint reload" state.
    connection->restart();

    pthread_mutex_lock(&connection_status_mutex);
    if (connection_status == CONNECTION_SUCCESS) {
        session->set_pause(saved_pause_cmd) ;
    }
    pthread_mutex_unlock(&connection_status_mutex);

    // Restart the variable server processing.
    pthread_mutex_unlock(&restart_pause);
}

void Trick::VariableServerThread::cleanup() {
    connection->disconnect();

    if (session != NULL)
        delete session;
}


