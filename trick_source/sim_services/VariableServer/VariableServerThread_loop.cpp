
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

    // Check for short running sims
    test_shutdown(NULL, NULL);

    //  We need to make the thread to VariableServerThread map before we accept the connection.
    //  Otherwise we have a race where this thread is unknown to the variable server and the
    //  client gets confirmation that the connection is ready for communication.
    vs->add_vst( pthread_self() , this ) ;

    // Accept client connection
    int status = connection->start();

    if (status != 0) {
        // TODO: Use a real error handler
        vs->delete_vst(pthread_self());

        // Tell main thread that we failed to initialize
        pthread_mutex_lock(&connection_status_mutex);
        connection_status = CONNECTION_FAIL;
        pthread_cond_signal(&connection_status_cv);
        pthread_mutex_unlock(&connection_status_mutex);

        cleanup();
        pthread_exit(NULL);
    }

    // Create session
    session = new VariableServerSession(connection);
    vs->add_session( pthread_self(), session );

    // Tell main that we are ready
    pthread_mutex_lock(&connection_status_mutex);
    connection_status = CONNECTION_SUCCESS;
    pthread_cond_signal(&connection_status_cv);
    pthread_mutex_unlock(&connection_status_mutex);

    // if log is set on for variable server (e.g., in input file), turn log on for each client
    if (vs->get_log()) {
        session->set_log_on();
    }

    try {
        while (1) {
            // Shutdown here if it's time
            test_shutdown(exit_var_thread, (void *) this);

            // Pause here if we are in a restart condition
            pthread_mutex_lock(&restart_pause) ;

            // Look for a message from the client
            // Parse and execute if one is availible
            session->handleMessage();

            // Check to see if exit is necessary
            if (session->exit_cmd == true) {
                pthread_mutex_unlock(&restart_pause) ;
                break;
            }

            // Copy data out of sim if async mode
            if ( session->get_copy_mode() == VS_COPY_ASYNC ) {
                session->copy_sim_data() ;
            }
    
            bool should_write_async = (session->get_write_mode() == VS_WRITE_ASYNC) || 
                                        ( session->get_copy_mode() == VS_COPY_ASYNC && (session->get_write_mode() == VS_WRITE_WHEN_COPIED)) || 
                                        (! is_real_time());

            // Write data out to connection if async mode and not paused
            if ( should_write_async && !session->get_pause()) {
                int ret = session->write_data() ;
                if ( ret < 0 ) {
                    pthread_mutex_unlock(&restart_pause) ;
                    break ;
                }
            }
            pthread_mutex_unlock(&restart_pause) ;

            usleep((unsigned int) (session->get_update_rate() * 1000000));
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
#if (__GNUC__ == 4 && __GNUC_MINOR__ == 1) || __GNUC__ == 12
        // for gcc 4.1.2 or whatever glib version in RHEL 5 that does not work with the abi::__forced_unwind
        // Also seems to have a problem with gcc 12
        throw;
#else
        message_publish(MSG_ERROR, "\nVARIABLE SERVER caught unknown exception\n" ) ;
        exit(-1) ;
#endif
#endif
#endif
    }

    if (debug >= 3) {
        message_publish(MSG_DEBUG, "%p tag=<%s> var_server receive loop exiting\n", connection, connection->getClientTag().c_str());
    }

    thread_shutdown(exit_var_thread, this);

    return NULL ;

}

