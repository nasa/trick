
#include <stdio.h>
#include <string.h>
#include <iostream>
#ifdef __linux
#include <cxxabi.h>
#endif
#include <netdb.h>


#include "trick/VariableServer.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/input_processor_proto.h"
#include "trick/tc_proto.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/realtimesync_proto.h"
#include "trick/ExecutiveException.hh"
#include "trick/exec_proto.h"

void exit_var_thread(void *in_vst) ;

void * Trick::VariableServerThread::thread_body() {

    int ii , jj;
    int msg_len;
    int ret;
    int nbytes = -1;
    char *last_newline ;
    unsigned int size ;
    socklen_t sock_size ;

    //  We need to make the thread to VariableServerThread map before we accept the connection.
    //  Otherwise we have a race where this thread is unknown to the variable server and the
    //  client gets confirmation that the connection is ready for communication.
    vs->add_vst( pthread_self() , this ) ;

    if ( listen_dev->socket_type == SOCK_STREAM ) {
        ret = tc_accept(listen_dev, &connection);
        tc_blockio(&connection, TC_COMM_ALL_OR_NOTHING);
    }
    connection_accepted = true ;

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL) ;
    pthread_cleanup_push(exit_var_thread, (void *) this);

    /* Save off the host and port information of the source port.  We want to ignore messages from
       this port if we are a multicast socket */
    struct sockaddr_in self_s_in ;
    int s_in_size =  sizeof(self_s_in) ;
    if ( conn_type == MCAST ) {
        getsockname( connection.socket , (struct sockaddr *)&self_s_in, (socklen_t *)&s_in_size) ;
        if ( self_s_in.sin_addr.s_addr == 0 ) {
            char hname[80];
            struct hostent *ip_host;
            gethostname(hname, (size_t) 80);
            ip_host = gethostbyname(hname);
            memcpy(&(self_s_in.sin_addr.s_addr), ip_host->h_addr, (size_t) ip_host->h_length);
        }
    }

    // if log is set on for variable server (e.g., in input file), turn log on for each client
    if (vs->get_log()) {
        log = true ;
    }

    try {
        while (1) {

            // Pause here if we are in a restart condition
            pthread_mutex_lock(&restart_pause) ;

            /* Check the length of the message on the socket */
            nbytes = recvfrom( connection.socket, incoming_msg, MAX_CMD_LEN, MSG_PEEK, NULL, NULL ) ;
            if (nbytes == 0 ) {
                break ;
            }

            if (nbytes != -1) { // -1 means socket is nonblocking and no data to read
                /* find the last newline that is present on the socket */
                incoming_msg[nbytes] = '\0' ;
                last_newline = rindex( incoming_msg , '\n') ;

                /* if there is a newline then there is a complete command on the socket */
                if ( last_newline != NULL ) {
                    /* only remove up to (and including) the last newline on the socket */
                    size = last_newline - incoming_msg + 1;
                    if ( conn_type == UDP ) {
                        // Save the remote host information, that is where we are going to send replies.
                        sock_size = sizeof(connection.remoteServAddr) ;
                        nbytes = recvfrom( connection.socket, incoming_msg, size, 0 ,
                         (struct sockaddr *)&connection.remoteServAddr, &sock_size ) ;
                    } else if ( conn_type == MCAST ) {
                        // Save the remove host information for test against ourself.
                        struct sockaddr_in s_in ;
                        sock_size = sizeof(s_in) ;
                        nbytes = recvfrom( connection.socket, incoming_msg, size, 0 ,
                         (struct sockaddr *)&s_in, &sock_size ) ;
                        // If this message is from us, then ignore it.
                        if ( s_in.sin_addr.s_addr == self_s_in.sin_addr.s_addr and s_in.sin_port == self_s_in.sin_port) {
                            nbytes = 0 ;
                        }
                    } else {
                        // We know where we are sending information, no need to save it.
                        nbytes = recvfrom( connection.socket, incoming_msg, size, 0 , NULL, NULL ) ;
                    }
                } else {
                    nbytes = 0 ;
                }
            }

            if ( nbytes > 0 ) {

                msg_len = nbytes ;
                if (debug >= 3) {
                    message_publish(MSG_DEBUG, "%p tag=<%s> var_server received bytes = msg_len = %d\n", &connection, connection.client_tag, nbytes);
                }

                incoming_msg[msg_len] = '\0' ;

                if (vs->get_info_msg() || (debug >= 1)) {
                    message_publish(MSG_DEBUG, "%p tag=<%s> var_server received: %s", &connection, connection.client_tag, incoming_msg) ;
                }
                if (log) {
                    message_publish(MSG_PLAYBACK, "tag=<%s> time=%f %s", connection.client_tag, exec_get_sim_time(), incoming_msg) ;
                }

                for( ii = 0 , jj = 0 ; ii <= msg_len ; ii++ ) {
                    if ( incoming_msg[ii] != '\r' ) {
                        stripped_msg[jj++] = incoming_msg[ii] ;
                    }
                }

                ip_parse(stripped_msg); /* returns 0 if no parsing error */

            }

            /* break out of loop if exit command found */
            if (exit_cmd == true) {
                break;
            }

            if ( copy_mode == VS_COPY_ASYNC ) {
                ret = copy_sim_data() ;
            }

            if ( (write_mode == VS_WRITE_ASYNC) or
                 ((copy_mode == VS_COPY_ASYNC) and (write_mode == VS_WRITE_WHEN_COPIED)) or
                 (! is_real_time()) ) {
                if ( !pause_cmd ) {
                    ret = write_data() ;
                    if ( ret < 0 ) {
                        break ;
                    }
                }
            }
            pthread_mutex_unlock(&restart_pause) ;

            usleep((unsigned int) (update_rate * 1000000));
        }
    } catch (Trick::ExecutiveException & ex ) {
        message_publish(MSG_ERROR, "\nVARIABLE SERVER COMMANDED exec_terminate\n  ROUTINE: %s\n  DIAGNOSTIC: %s\n" ,
         ex.file.c_str(), ex.message.c_str()) ;
        exit(ex.ret_code) ;
    } catch (const std::exception &ex) {
        message_publish(MSG_ERROR, "\nVARIABLE SERVER caught std::exception\n  DIAGNOSTIC: %s\n" ,
         ex.what()) ;
        exit(-1) ;
#ifdef __linux
#ifdef __GNUC__
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
    // for post gcc 4.1.2 or whatever glibc version is used in RHEL6 and above.
    } catch (abi::__forced_unwind&) {
        //pthread_exit and pthread_cancel will cause an abi::__forced_unwind to be thrown. Rethrow it.
        throw;
#endif
#endif
#endif
    } catch (...) {
#ifdef __linux
#ifdef __GNUC__
#if __GNUC__ == 4 && __GNUC_MINOR__ == 1
        //for gcc 4.1.2 or whatever glib version in RHEL 5 that does not work with the abi::__forced_unwind
        throw;
#else
        message_publish(MSG_ERROR, "\nVARIABLE SERVER caught unknown exception\n" ) ;
        exit(-1) ;
#endif
#endif
#endif
    }

    if (debug >= 3) {
        message_publish(MSG_DEBUG, "%p tag=<%s> var_server receive loop exiting\n", &connection, connection.client_tag);
    }

    pthread_cleanup_pop(1);
    pthread_exit(NULL) ;
    return NULL ;

}

