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
#include "trick/ThreadBase.hh"
// #include "trick/VariableReference.hh"
#include "trick/VariableServerSession.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/variable_server_message_types.h"


namespace Trick {

    class VariableServer ;

/**
  This class provides variable server command processing on a separate thread for each client.
  @author Alex Lin
 */
    class VariableServerThread : public Trick::ThreadBase {

        public:
            // Are UDP or MCAST ever actually used?
            enum ConnectionType { TCP, UDP, MCAST } ;

            // Is this ever actually used?
            friend std::ostream& operator<< (std::ostream& s, Trick::VariableServerThread& vst);

            /**
             @brief Constructor.
             @param listen_dev - the TCDevice set up in listen()
            */
            VariableServerThread(TCDevice * in_listen_dev ) ;

            virtual ~VariableServerThread() ;
            /**
             @brief static routine called from S_define to set the VariableServer pointer for all threads.
             @param in_vs - the master variable server object
            */
            static void set_vs_ptr(Trick::VariableServer * in_vs) ;

            /**
             @brief Wait for the connection_accepted flag to be set.
            */
            void wait_for_accept() ;

            /**
             @brief The main loop of the variable server thread that reads and processes client commands.
             @return always 0
            */
            virtual void * thread_body() ;

            VariableServer * get_vs() ;

            /**
             @brief Internal function used by input processor to send stdout and stderr to the client.
             @return always 0
            */
            // int write_stdio(int stream , std::string text ) ;

            // int freeze_init() ;

            // int copy_data_freeze() ;
            // int copy_data_freeze_scheduled(long long curr_tics) ;
            // int copy_data_scheduled(long long curr_tics) ;
            // int copy_data_top() ;

            void preload_checkpoint() ;

            // Is this restarting thread itself or session?
            void restart() ;

            // long long get_next_tics() ;
            // long long get_freeze_next_tics() ;

            // Do these actually get used?
            // Mcast seems like no
            // not sure about udp
            /**
             @brief creates a udp socket with the specified address and port.  UDP ports do not go through
             the listen and accept states
            */
            int create_udp_socket(const char * address, unsigned short in_port) ;

            /**
             @brief creates a multicast socket with the specified address and port.  mcast ports do not go through
             the listen and accept states
            */
            int create_mcast_socket(const char * mcast_address, const char * address, unsigned short in_port) ;

            TCDevice & get_connection();

        protected:

            /**
             @brief Called by send_sie commands to transmit files through the socket.
            */
            int transmit_file(std::string file_name);

            /** The Master variable server object. */
            static VariableServer * vs ;

            /** this is where a lot of this should happen now */
            VariableServerSession * session;

            /** The listen device from the variable server\n */
            TCDevice * listen_dev;          /**<  trick_io(**) */
            TCDevice connection;          /**<  trick_io(**) */

            /** The type of connection we have.\n */
            ConnectionType conn_type ;      /**<  trick_io(**) */

            /** Value (1,2,or 3) that causes the variable server to output increasing amounts of debug information.\n */
            int debug ;                      /**<  trick_io(**) */

            /** Toggle to enable/disable this variable server thread.\n */
            bool enabled ;                   /**<  trick_io(**) */

            // /** Toggle to turn on/off variable server logged messages to a playback file.\n */
            // bool log ;                       /**< trick_io(**)  */

            /** Toggle to indicate var_pause commanded.\n */
            bool pause_cmd ;                 /**<  trick_io(**) */

            /** Save pause state while reloading a checkpoint.\n */
            bool saved_pause_cmd ;           /**<  trick_io(**) */

            /** Toggle to indicate var_exit commanded.\n */
            bool exit_cmd ;                  /**<  trick_io(**) */

            /** Set to true to validate all addresses before copying.\n */
            // bool validate_address ;          /**<  trick_io(**) */

            /** The mutex to protect variable output buffers when copying variable values to them from Trick memory.\n */
            pthread_mutex_t copy_mutex ;     /**<  trick_io(**) */

            /** The mutex pauses all processing during checkpoint restart */
            pthread_mutex_t restart_pause ;     /**<  trick_io(**) */

            /** The simulation time converted to seconds\n */
            double time ;                    /**<  trick_units(s) */

            /** Toggle to set variable server copy as top_of_frame, scheduled, async \n */
            // VS_COPY_MODE copy_mode ;         /**<  trick_io(**) */

            // /** Toggle to set variable server writes as when copied or async.\n */
            // VS_WRITE_MODE write_mode ;       /**<  trick_io(**) */

            // // /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            // int frame_multiple ;             /**<  trick_io(**) */

            // // /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            // int frame_offset ;               /**<  trick_io(**) */

            // // /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            // int freeze_frame_multiple ;      /**<  trick_io(**) */

            // // /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            // int freeze_frame_offset ;        /**<  trick_io(**) */

            /** Toggle to tell variable server to byteswap returned values.\n */
            // bool byteswap ;                  /**<  trick_io(**) */

            // /** Toggle to tell variable server return data in binary format.\n */
            // bool binary_data ;               /**<  trick_io(**) */

            // /** Toggle to tell variable server return data in binary format without the variable names.\n */
            // bool binary_data_nonames ;       /**<  trick_io(**) */

            /** Toggle to tell variable server to send data multicast or point to point.\n */
            bool multicast ;                 /**<  trick_io(**) */

            /** Flag to indicate the connection has been made\n */
            bool connection_accepted ;       /**<  trick_io(**) */

            

            /** Indicate whether variable data has been written into buffer_in.\n */
            // bool var_data_staged;           /**<  trick_io(**) */

            // Is this used?
            /** number of packets copied to client \n */
            // unsigned int packets_copied ;   /**< trick_io(**) */

            /** Toggle to indicate sending python stdout and stderr to client\n */
            // bool send_stdio ;                /**<  trick_io(**) */

            // /** Holding area for the incoming message\n */
            // char *incoming_msg;           /**< trick_io(**) */

            // /** Message with '\r' characters removed\n */
            // char *stripped_msg;           /**<  trick_io(**) */

            /** Maximum size of incoming message\n */
            // static const unsigned int MAX_CMD_LEN = 200000 ;
    } ;
}

#endif

