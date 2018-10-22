
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
 Trick::ThreadBase("VarServListen"),
 port(0),
 user_port_requested(false),
 broadcast(true),
 listen_dev()
{
    char hname[80];
    gethostname(hname , (size_t) 80 ) ;
    source_address = std::string(hname) ;
    strcpy(listen_dev.client_tag, "<empty>");
    tc_error(&listen_dev, 0);
    pthread_mutex_init(&restart_pause, NULL);
}

Trick::VariableServerListenThread::~VariableServerListenThread() {
        free(listen_dev.hostname) ;
        free(listen_dev.error_handler) ;
}

const char * Trick::VariableServerListenThread::get_hostname() {
    return listen_dev.hostname;
}

unsigned short Trick::VariableServerListenThread::get_port() {
    return port;
}

void Trick::VariableServerListenThread::set_port(unsigned short in_port) {
    port = in_port;
    user_port_requested = true ;
}

std::string Trick::VariableServerListenThread::get_user_tag() {
    return user_tag ;
}

void Trick::VariableServerListenThread::set_user_tag(std::string in_tag) {
    user_tag = in_tag ;
}

void Trick::VariableServerListenThread::set_source_address(const char * address) {
    if ( address == NULL ) {
        source_address = std::string("") ;
    } else {
        source_address = std::string(address) ;
    }
    user_port_requested = true ;
}

std::string Trick::VariableServerListenThread::get_source_address() {
    return source_address ;
}

bool Trick::VariableServerListenThread::get_broadcast() {
    return broadcast;
}

void Trick::VariableServerListenThread::set_broadcast(bool in_broadcast) {
    broadcast = in_broadcast;
}

int Trick::VariableServerListenThread::init_listen_device() {
    int ret;

    ret = tc_init(&listen_dev);
    if (ret == TC_SUCCESS) {
        port = listen_dev.port ;
    } else {
        fprintf(stderr, "ERROR: Could not establish listen port for Variable Server. Aborting.\n");
        ret = -1 ;
    }
    return ret ;
}

int Trick::VariableServerListenThread::check_and_move_listen_device() {
    int ret ;

    if ( source_address.compare(listen_dev.hostname) or port != listen_dev.port ) {
        /* The user has requested a different source address or port in the input file */
        tc_disconnect(&listen_dev) ;
        ret = tc_init_with_connection_info(&listen_dev, AF_INET, SOCK_STREAM, source_address.c_str(), port) ;
        if (ret != TC_SUCCESS) {
            message_publish(MSG_ERROR, "ERROR: Could not establish variable server source_address %s: port %d. Aborting.\n",
             source_address.c_str(), port);
            return -1 ;
        }
    }
    return 0 ;
}

void Trick::VariableServerListenThread::create_tcp_socket(const char * address, unsigned short in_port ) {
    tc_init_with_connection_info(&listen_dev, AF_INET, SOCK_STREAM, address, in_port) ;
}

void * Trick::VariableServerListenThread::thread_body() {

    fd_set rfds;
    struct timeval timeout_time = { 2, 0 };
    char buf1[1024] = { 0 } ;
    struct passwd * passp ;
    Trick::VariableServerThread * vst ;
    int value;
    std::string version;
    char * user_name ;

    int mcast_socket ;
    struct sockaddr_in mcast_addr ;
    struct sockaddr_in mcast_addr_legacy ;

    version = std::string(exec_get_current_version()) ;
    version.erase(version.find_last_not_of(" \t\f\v\n\r")+1);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    passp = getpwuid(getuid()) ;

    if ( passp == NULL ) {
        user_name = strdup("unknown") ;
    } else {
        user_name = strdup(passp->pw_name) ;
    }

    tc_blockio(&listen_dev, TC_COMM_BLOCKIO);

    if ( broadcast ) {
        if ((mcast_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("vs_mcast_init socket");
        }

        value = 1;
        if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &value, (socklen_t) sizeof(value)) < 0) {
            perror("setsockopt: reuseaddr");
        }
#ifdef SO_REUSEPORT
        if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEPORT, (char *) &value, sizeof(value)) < 0) {
            perror("setsockopt: reuseport");
        }
#endif

        /* Set up destination address */
        memset(&mcast_addr, 0, sizeof(mcast_addr));
        mcast_addr.sin_family = AF_INET;
        mcast_addr.sin_addr.s_addr = inet_addr("239.3.14.15");
        mcast_addr.sin_port = htons((uint16_t) 9265);

        memset(&mcast_addr_legacy, 0, sizeof(mcast_addr_legacy));
        mcast_addr_legacy.sin_family = AF_INET;
        mcast_addr_legacy.sin_addr.s_addr = inet_addr("224.3.14.15");
        mcast_addr_legacy.sin_port = htons((uint16_t) 9265);
    }

    while (1) {

        FD_ZERO(&rfds);
        FD_SET(listen_dev.socket, &rfds);
        timeout_time.tv_sec = 2 ;
        select(listen_dev.socket + 1, &rfds, NULL, NULL, &timeout_time);

        if (FD_ISSET(listen_dev.socket, &rfds)) {
            // pause here during restart
            pthread_mutex_lock(&restart_pause) ;
            vst = new Trick::VariableServerThread(&listen_dev) ;
            vst->copy_cpus(get_cpus()) ;
            vst->create_thread() ;
            vst->wait_for_accept() ;
            pthread_mutex_unlock(&restart_pause) ;
        } else {
            if ( broadcast ) {
                sprintf(buf1 , "%s\t%hu\t%s\t%d\t%s\t%s\t%s\t%s\t%s\t%hu\n" , listen_dev.hostname , (unsigned short)listen_dev.port ,
                 user_name , (int)getpid() , command_line_args_get_default_dir() , command_line_args_get_cmdline_name() ,
                 command_line_args_get_input_file() , version.c_str() , user_tag.c_str(), (unsigned short)listen_dev.port ) ;

                sendto(mcast_socket , buf1 , strlen(buf1) , 0 , (struct sockaddr *)&mcast_addr , (socklen_t)sizeof(mcast_addr)) ;
                sendto(mcast_socket , buf1 , strlen(buf1) , 0 , (struct sockaddr *)&mcast_addr_legacy , (socklen_t)sizeof(mcast_addr)) ;
            }
        }

    }

    return NULL ;
}

#include <fcntl.h>

int Trick::VariableServerListenThread::restart() {

    int ret ;

    if ( user_port_requested ) {

        char hname[80];
        static struct sockaddr_in s_in;
        gethostname(hname, (size_t) 80);
        // Test to see if the restart address is on this machine.  If it is not, it's not an error, clear source address
        if ( strcmp( source_address.c_str(), hname )) {
            if (! inet_pton(AF_INET, source_address.c_str(), (struct in_addr *)&s_in.sin_addr.s_addr) ) {
                //printf("clearing source_address\n") ;
                source_address.clear() ;
            }
        }

        printf("variable server restart user_port requested set %d\n", port) ;

        tc_disconnect(&listen_dev) ;
        ret = tc_init_with_connection_info(&listen_dev, AF_INET, SOCK_STREAM, source_address.c_str(), port) ;
        if (ret != TC_SUCCESS) {
            message_publish(MSG_ERROR, "ERROR: Could not establish listen port %d for Variable Server. Aborting.\n", port);
            return (-1);
        }
    } else {
        struct sockaddr_in s_in;
        int s_in_size =  sizeof(s_in) ;
        getsockname( listen_dev.socket , (struct sockaddr *)&s_in, (socklen_t *)&s_in_size) ;
        printf("restart variable server message port = %d\n" , ntohs(s_in.sin_port)) ;
        port = listen_dev.port = ntohs(s_in.sin_port);
    }

    return 0 ;
}

void Trick::VariableServerListenThread::pause_listening() {
    pthread_mutex_lock(&restart_pause) ;
}

void Trick::VariableServerListenThread::restart_listening() {
    pthread_mutex_unlock(&restart_pause) ;
}

void Trick::VariableServerListenThread::dump( std::ostream & oss ) {
    oss << "Trick::VariableServerListenThread (" << name << ")" << std::endl ;
    oss << "    source_address = " << source_address << std::endl ;
    oss << "    port = " << port << std::endl ;
    oss << "    user_port_requested = " << user_port_requested << std::endl ;
    oss << "    user_tag = " << user_tag << std::endl ;
    oss << "    broadcast = " << broadcast << std::endl ;
    Trick::ThreadBase::dump(oss) ;
}

