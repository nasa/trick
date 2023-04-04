
#include <iostream>
#include <stdlib.h>
#include "trick/VariableServerThread.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/TrickConstant.hh"
#include "trick/UDPConnection.hh"
#include "trick/TCPConnection.hh"


Trick::VariableServer * Trick::VariableServerThread::_vs = NULL ;

static int instance_num = 0;

Trick::VariableServerThread::VariableServerThread() : VariableServerThread (new VariableServerSession()) {}

Trick::VariableServerThread::VariableServerThread(VariableServerSession * session) :
 Trick::SysThread(std::string("VarServer" + std::to_string(instance_num++))) , _debug(0), _session(session), _connection(NULL) {

    _connection_status = CONNECTION_PENDING ;


    pthread_mutex_init(&_connection_status_mutex, NULL);
    pthread_cond_init(&_connection_status_cv, NULL);

    cancellable = false;
}

Trick::VariableServerThread::~VariableServerThread() {
    cleanup();
}

std::ostream& Trick::operator<< (std::ostream& s, Trick::VariableServerThread& vst) {
    // Write a JSON representation of a Trick::VariableServerThread to an ostream.


    s << "  \"connection\":{\n";
    s << "    \"client_tag\":\"" << vst._connection->getClientTag() << "\",\n";

    s << "    \"client_IP_address\":\"" << vst._connection->getClientHostname() << "\",\n";
    s << "    \"client_port\":\"" << vst._connection->getClientPort() << "\",\n";

    pthread_mutex_lock(&vst._connection_status_mutex);
    if (vst._connection_status == CONNECTION_SUCCESS) {
        s << *(vst._session);
    } else {
        
    }
    pthread_mutex_unlock(&vst._connection_status_mutex);

    s << "  }" << std::endl;
    return s;
}

void Trick::VariableServerThread::set_vs_ptr(Trick::VariableServer * in_vs) {
    _vs = in_vs ;
}

Trick::VariableServer * Trick::VariableServerThread::get_vs() {
    return _vs ;
}

void Trick::VariableServerThread::set_client_tag(std::string tag) {
    _connection->setClientTag(tag);
}

void Trick::VariableServerThread::set_connection(Trick::ClientConnection * in_connection) {
    _connection = in_connection;
}

Trick::ConnectionStatus Trick::VariableServerThread::wait_for_accept() {

    pthread_mutex_lock(&_connection_status_mutex);
    while ( _connection_status == CONNECTION_PENDING ) {
        pthread_cond_wait(&_connection_status_cv, &_connection_status_mutex);
    }
    pthread_mutex_unlock(&_connection_status_mutex);

    return _connection_status;
}

// Gets called from the main thread as a job
void Trick::VariableServerThread::preload_checkpoint() {

    // Stop variable server processing at the top of the processing loop.
    force_thread_to_pause();


    // Make sure that the _session has been initialized
    pthread_mutex_lock(&_connection_status_mutex);
    if (_connection_status == CONNECTION_SUCCESS) {

        // Let the thread complete any data copying it has to do
        // and then suspend data copying until the checkpoint is reloaded.
        _session->pause_copy();
        
        // Save the pause state of this thread.
        _saved_pause_cmd = _session->get_pause();

        // Disallow data writing.
        _session->set_pause(true);

        // Temporarily "disconnect" the variable references from Trick Managed Memory
        // by tagging each as a "bad reference".
        _session->disconnect_references();

        // Allow data copying to continue.
        _session->unpause_copy();
    }
    pthread_mutex_unlock(&_connection_status_mutex);
}

// Gets called from the main thread as a job
void Trick::VariableServerThread::restart() {
    // Set the pause state of this thread back to its "pre-checkpoint reload" state.
    _connection->restart();

    pthread_mutex_lock(&_connection_status_mutex);
    if (_connection_status == CONNECTION_SUCCESS) {
        _session->set_pause(_saved_pause_cmd) ;
    }
    pthread_mutex_unlock(&_connection_status_mutex);

    // Restart the variable server processing.
    unpause_thread();
}

void Trick::VariableServerThread::cleanup() {
    _connection->disconnect();

    if (_session != NULL) {
        delete _session;
        _session = NULL;
    }
}

