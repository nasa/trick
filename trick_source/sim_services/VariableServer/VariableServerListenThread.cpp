
#include <iostream>
#include <pwd.h>

#include "trick/VariableServerListenThread.hh"
#include "trick/VariableServerThread.hh"
#include "trick/tc_proto.h"
#include "trick/exec_proto.h"
#include "trick/command_line_protos.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"


Trick::VariableServerListenThread::VariableServerListenThread() :
 Trick::SysThread("VarServListen"),
 requested_port(0),
 requested_source_address(""),
 user_requested_address(false),
 broadcast(true),
 listener()
{
    pthread_mutex_init(&restart_pause, NULL);
}

Trick::VariableServerListenThread::~VariableServerListenThread() {
    // if (multicast != NULL) {
    //     delete multicast;
    //     multicast = NULL;
    // }
}


const char * Trick::VariableServerListenThread::get_hostname() {
    const char * ret = listener.getHostname();
    return ret;
}

unsigned short Trick::VariableServerListenThread::get_port() {
    return requested_port;
}

void Trick::VariableServerListenThread::set_port(unsigned short in_port) {
    requested_port = in_port;
    user_requested_address = true ;
}

std::string Trick::VariableServerListenThread::get_user_tag() {
    return user_tag ;
}

const std::string& Trick::VariableServerListenThread::get_user_tag_ref() {
    return user_tag ;
}

void Trick::VariableServerListenThread::set_user_tag(std::string in_tag) {
    user_tag = in_tag ;
}

void Trick::VariableServerListenThread::set_source_address(const char * address) {
    if ( address == NULL ) {
        requested_source_address = std::string("") ;
    } else {
        requested_source_address = std::string(address) ;
    }
    user_requested_address = true ;
}

std::string Trick::VariableServerListenThread::get_source_address() {
    return listener.getHostname() ;
}

bool Trick::VariableServerListenThread::get_broadcast() {
    return broadcast;
}

// in_broadcast atomic? We'll see what tsan says. Maybe go nuts with atomics
void Trick::VariableServerListenThread::set_broadcast(bool in_broadcast) {
    broadcast = in_broadcast;
}

int Trick::VariableServerListenThread::init_listen_device() {
    int ret = listener.initialize();
    requested_port = listener.getPort();
    requested_source_address = std::string(listener.getHostname());
    return ret;
}

int Trick::VariableServerListenThread::check_and_move_listen_device() {
    int ret ;

    if ( user_requested_address ) {

        /* The user has requested a different source address or port in the input file */
        listener.disconnect();
        ret = listener.initialize(requested_source_address, requested_port);
        requested_port = listener.getPort();
        requested_source_address = std::string(listener.getHostname());
        if (ret != TC_SUCCESS) {
            std::cout << "Unsuccessful initialization " << std::endl;
            message_publish(MSG_ERROR, "ERROR: Could not establish variable server source_address %s: port %d. Aborting.\n",
             requested_source_address.c_str(), requested_port);
            return -1 ;
        }
    }

    return 0 ;
}

void Trick::VariableServerListenThread::create_tcp_socket(const char * address, unsigned short in_port ) {
    listener.initialize(address, in_port);
}

void * Trick::VariableServerListenThread::thread_body() {
    // This thread listens for incoming client connections, and when one is received, creates a new thread to handle the session
    // Also broadcasts on multicast channel

    std::string version = std::string(exec_get_current_version()) ;
    version.erase(version.find_last_not_of(" \t\f\v\n\r")+1);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // get username to broadcast on multicast channel
    struct passwd *passp = getpwuid(getuid()) ;
    std::string user_name;
    if ( passp == NULL ) {
        user_name = strdup("unknown") ;
    } else {
        user_name = strdup(passp->pw_name) ;
    }
    
    listener.setBlockMode(TC_COMM_BLOCKIO);

    if ( broadcast ) {
        initializeMulticast();
    }

    while (1) {

        // Look for a new client requesting a connection
        if (listener.checkForNewConnections()) {
            // pause here during restart
            pthread_mutex_lock(&restart_pause) ;

            // Create a new thread to service this connection
            VariableServerThread * vst = new Trick::VariableServerThread(&listener) ;
            vst->copy_cpus(get_cpus()) ;
            vst->create_thread() ;
            vst->wait_for_accept() ;

            pthread_mutex_unlock(&restart_pause) ;
        } else if ( broadcast ) {
<<<<<<< HEAD
            snprintf(buf1 , sizeof(buf1), "%s\t%hu\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%hu\n" , listen_dev.hostname , (unsigned short)listen_dev.port ,
             user_name , (int)getpid() , command_line_args_get_default_dir() , command_line_args_get_cmdline_name() ,
             command_line_args_get_input_file() , version.c_str() , user_tag.c_str(), (unsigned short)listen_dev.port ) ;
=======
            // Otherwise, broadcast on the multicast channel if enabled
            char buf1[1024];
            sprintf(buf1 , "%s\t%hu\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%hu\n" , listener.getHostname(), (unsigned short)listener.getPort() ,
             user_name.c_str() , (int)getpid() , command_line_args_get_default_dir() , command_line_args_get_cmdline_name() ,
             command_line_args_get_input_file() , version.c_str() , user_tag.c_str(), (unsigned short)listener.getPort() ) ;
>>>>>>> e8ba4328 (Encapsulated listen device into ClientListener)

            std::string message = buf1;

            if (!multicast.is_initialized()) {
                // In case broadcast was turned on after this loop was entered
                initializeMulticast();
            }
            multicast.broadcast(message);
        }
    }

    return NULL ;
}

#include <fcntl.h>

int Trick::VariableServerListenThread::restart() {

    int ret ;

    if ( user_requested_address ) {

        if (!listener.validateSourceAddress(requested_source_address)) {
            requested_source_address.clear() ;
        }

        listener.disconnect();
        ret = listener.initialize(requested_source_address, requested_port);
        
        if (ret != TC_SUCCESS) {
            message_publish(MSG_ERROR, "ERROR: Could not establish listen port %d for Variable Server. Aborting.\n", requested_port);
            return (-1);
        }
    } else {
        listener.checkSocket();
    }

    initializeMulticast();

    return 0 ;
}

void Trick::VariableServerListenThread::initializeMulticast() {
    multicast.initialize();
    multicast.addAddress("239.3.14.15", 9265);
    multicast.addAddress("224.3.14.15", 9265);
}

void Trick::VariableServerListenThread::pause_listening() {
    pthread_mutex_lock(&restart_pause) ;
}

void Trick::VariableServerListenThread::restart_listening() {
    pthread_mutex_unlock(&restart_pause) ;
}

void Trick::VariableServerListenThread::dump( std::ostream & oss ) {
    oss << "Trick::VariableServerListenThread (" << name << ")" << std::endl ;
    oss << "    source_address = " << listener.getHostname() << std::endl ;
    oss << "    port = " << listener.getPort() << std::endl ;
    oss << "    user_requested_address = " << user_requested_address << std::endl ;
    oss << "    user_tag = " << user_tag << std::endl ;
    oss << "    broadcast = " << broadcast << std::endl ;
    Trick::ThreadBase::dump(oss) ;
}

