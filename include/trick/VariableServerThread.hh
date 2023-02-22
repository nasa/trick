/*
    PURPOSE:
        (VariableServerThread)
*/

#ifndef VARIABLESERVERTHREAD_HH
#define VARIABLESERVERTHREAD_HH

#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>
#include "trick/tc.h"
#include "trick/SysThread.hh"
#include "trick/VariableServerSession.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/variable_server_message_types.h"

#include "trick/ClientConnection.hh"
#include "trick/ClientListener.hh"

namespace Trick {

    class VariableServer ;

    /** Flag to indicate the connection has been made\n */
    enum ConnectionStatus { CONNECTION_PENDING, CONNECTION_SUCCESS, CONNECTION_FAIL };


/**
  This class provides variable server command processing on a separate thread for each client.
  @author Alex Lin
 */
    class VariableServerThread : public Trick::SysThread {

        public:
            friend std::ostream& operator<< (std::ostream& s, Trick::VariableServerThread& vst);

            /**
             @brief Constructor.
             @param listen_dev - the TCDevice set up in listen()
            */
            VariableServerThread() ;

            virtual ~VariableServerThread() ;
            /**
             @brief static routine called from S_define to set the VariableServer pointer for all threads.
             @param in_vs - the master variable server object
            */
            static void set_vs_ptr(Trick::VariableServer * in_vs) ;

            void set_client_tag(std::string tag);

            /**
             @brief Open a UDP socket for this thread
            */
            int open_udp_socket(const std::string& hostname, int port);

            /**
             @brief Open a TCP connection for this thread
            */
            int open_tcp_connection(ClientListener * listener);

            /**
             @brief Block until thread has accepted connection
            */
            ConnectionStatus wait_for_accept() ;

            /**
             @brief The main loop of the variable server thread that reads and processes client commands.
             @return always 0
            */
            virtual void * thread_body() ;

            VariableServer * get_vs() ;

            void preload_checkpoint() ;

            void restart() ;

            void cleanup();

        protected:

            /**
             @brief Called by send_sie commands to transmit files through the socket.
            */
            int transmit_file(std::string file_name);

            /** The Master variable server object. */
            static VariableServer * vs ;

            /** Manages the variable list  */
            VariableServerSession * session;       /**<  trick_io(**) */

            /** Connection to the client */
            ClientConnection * connection;        /**<  trick_io(**) */

            /** Value (1,2,or 3) that causes the variable server to output increasing amounts of debug information.\n */
            int debug ;                      /**<  trick_io(**) */

            /** Toggle to enable/disable this variable server thread.\n */
            bool enabled ;                   /**<  trick_io(**) */

            ConnectionStatus connection_status ;       /**<  trick_io(**) */
            pthread_mutex_t connection_status_mutex;     /**<  trick_io(**) */
            pthread_cond_t connection_status_cv;         /**<  trick_io(**) */

            /** The mutex pauses all processing during checkpoint restart */
            pthread_mutex_t restart_pause ;     /**<  trick_io(**) */

            // bool pause_cmd;
            bool saved_pause_cmd;

            /** The simulation time converted to seconds\n */
            double time ;                    /**<  trick_units(s) */

            /** Toggle to tell variable server to send data multicast or point to point.\n */
            bool multicast ;                 /**<  trick_io(**) */
    } ;

    std::ostream& operator<< (std::ostream& s, VariableServerThread& vst);

}

#endif

