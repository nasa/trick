/*
    PURPOSE:
        (VariableServerThread)
*/

#ifndef VARIABLESERVERTHREAD_HH
#define VARIABLESERVERTHREAD_HH

#include <vector>
#include <string>
#include <pthread.h>
#include "trick/tc.h"
#include "trick/ThreadBase.hh"
#include "trick/VariableServerReference.hh"
#include "trick/variable_server_sync_types.h"

namespace Trick {

    class VariableServer ;

/**
  This class provides variable server command processing on a separate thread for each client.
  @author Alex Lin
 */
    class VariableServerThread : public Trick::ThreadBase {

        public:
            enum ConnectionType { TCP, UDP, MCAST } ;

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

            /**
             @brief @userdesc Command to add a variable to a list of registered variables for value retrieval.
             The variable server will immediately begin returning the variable values to the client at a
             frequency according to var_cycle.
             @par Python Usage:
             @code trick.var_add("<in_name>") @endcode
             @param in_name - the variable name to retrieve
             @return always 0
            */
            int var_add( std::string in_name ) ;

            /**
             @brief @userdesc Command to add a variable to a list of registered variables for value retrieval,
             and also instructs the variable server to return the value in the specified measurement units.
             The unit specification will be returned after the variable's value (var_ascii mode only).
             Specifying units as "{xx}" will use the unit specification from the variable's model code declaration,
             which is essentially a method of querying a variable's unit specification.
             The variable server will immediately begin returning the variable values/units to the client at
             a frequency according to var_cycle.
             @par Python Usage:
             @code trick.var_add("<in_name>", "<units_name>") @endcode
             @param in_name - the variable name to retrieve
             @param units_name - the desired units, specified within curly braces
             @return always 0
            */
            int var_add( std::string in_name, std::string units_name ) ;

            /**
             @brief @userdesc Command to remove a variable (previously registered with var_add)
             from the list of registered variables for value retrieval.
             @par Python Usage:
             @code trick.var_remove("<in_name>") @endcode
             @param in_name - the variable name to remove
             @return always 0
            */
            int var_remove( std::string in_name ) ;

            /**
             @brief @userdesc Command to instruct the variable server to return the value of a variable
             in the specified units (var_ascii mode only).
             The variable must have been previously registered with the var_add command.
             The unit specification will be returned after the variable's value.
             @par Python Usage:
             @code trick.var_units("<var_name>", "<units_name>") @endcode
             @param var_name - the variable name previously registered with var_add
             @param units_name - the desired units, specified within curly braces
             @return always 0
             @note trick.var_add("my_object.my_variable"); trick.var_units("my_object.my_variable","{m}")
             is the same as trick.var_add("my_object.my_variable","{m}")
            */
            int var_units(std::string var_name , std::string units_name) ;

            /**
             @brief @userdesc Command to instruct the variable server to send a Boolean value indicating
             whether the specified variable exists
             (is a Trick processed variable in the current simulation).
             - var_binary mode: the message indicator is 1, and the returned value is a binary 0 or 1.
             - var_ascii mode: the message indicator is "1" followed by a tab, then an ASCII "0" or "1" returned value.
             .
             @par Python Usage:
             @code trick.var_exists("<in_name>") @endcode
             @param in_name - the variable name to query
             @return always 0
            */
            int var_exists( std::string in_name ) ;

            /**
             @brief @userdesc Command to instruct the variable server to immediately send back the values of
             variables that are registered with the var_add command
             (typically used when var_pause is in effect). Each var_send command sends back all current values
             once instead of cyclically.
             @par Python Usage:
             @code trick.var_send() @endcode
             @return always 0
            */
            int var_send() ;

            /**
             @brief @userdesc Command to remove all variables from the list of variables currently
             registered with the var_add command,
             so the variable server will no longer send cyclic values until new var_add commands are issued.
             @par Python Usage:
             @code trick.var_clear() @endcode
             @return always 0
            */
            int var_clear() ;

            /**
             @brief @userdesc Command to set the frequencty at which the variable server will send values
             of variables that have been registered using
             the var_add command. If var_cycle is not specified, the default cycle is 0.1 seconds.
             @par Python Usage:
             @code trick.var_cycle(<in_cycle>) @endcode
             @param in_cycle - the desired frequency in seconds
             @return always 0
            */
            int var_cycle(double in_cycle) ;

            /**
             @brief Get the pause state of this thread.
            */
            bool get_pause() ;

            /**
             @brief Set the pause state of this thread.
            */
            void set_pause(bool on_off) ;

            /**
             @brief @userdesc Command to terminate the current variable server/client connection.
             @par Python Usage:
             @code trick.var_exit() @endcode
             @return always 0
            */
            int var_exit() ;

            /**
             @brief @userdesc Turns on validating addresses before they are referenced
             @par Python Usage:
             @code trick.var_validate_address() @endcode
             @return always 0
            */
            int var_validate_address(bool on_off) ;

            /**
             @brief @userdesc Command to instruct the variable server to output debug information.
             @par Python Usage:
             @code trick.var_debug(<level>) @endcode
             @return always 0
             @param level - 1,2,or 3, higher number increases amount of info output
            */
            int var_debug(int level) ;

            /**
             @brief @userdesc Command to instruct the variable server to return values in ASCII format (this is the default).
             @par Python Usage:
             @code trick.var_ascii() @endcode
             @return always 0
            */
            int var_ascii() ;

            /**
             @brief @userdesc Command to instruct the variable server to return values in binary format.
             @par Python Usage:
             @code trick.var_binary() @endcode
             @return always 0
            */
            int var_binary() ;

            /**
             @brief @userdesc Command to instruct the variable server to return values in binary format,
             but saves space in the returned messages
             by omitting the variable names.
             @par Python Usage:
             @code trick.var_binary_nonames() @endcode
             @return always 0
            */
            int var_binary_nonames() ;

            /**
             @brief @userdesc Command to tell the server when to copy data
             - VS_COPY_ASYNC = copies data asynchronously. (default)
             - VS_COPY_SCHEDULED = copies data as an automatic_last job in main thread
             - VS_COPY_TOP_OF_FRAME = copies data at top of frame
             @par Python Usage:
             @code trick.var_set_copy_mode(<mode>) @endcode
             @param mode - One of the above enumerations
             @return 0 if successful, -1 if error
            */
            int var_set_copy_mode(int on_off) ;

            /**
             @brief @userdesc Command to tell the server when to copy data
             - VS_WRITE_ASYNC = writes data asynchronously. (default)
             - VS_WRITE_WHEN_COPIED = writes data as soon as it's copied from sim
             @par Python Usage:
             @code trick.var_set_write_mode(<mode>) @endcode
             @param mode - One of the above enumerations
             @return 0 if successful, -1 if error
            */
            int var_set_write_mode(int on_off) ;

            /**
             @brief @userdesc Command to put the current variable server/client connection in sync mode,
             so that values to be sent to the client
             are guaranteed to be homogenous, that is from the same execution frame (the default is asynchronous).
             - async/async mode: the variable server thread itself copies the client data,
               independent of the execution frame so may not be homogenous.
             - sync/async mode: a variable server automatic_last job retrieves client requested data.
               Data is sent asynchronously on separate thread.
             - sync/sync mode: a variable server automatic_last job retrieves client requested data.
               Data is sent in same automatic_last job.
             .
             @par Python Usage:
             @code trick.var_sync(<on_off>) @endcode
             @param on_off - 0 = fully asynchronous. 1 = sync data gather, async socket write.
              2 = sync data gather, sync socket write
             @return always 0
            */
            int var_sync(int on_off) ;

            /**
             @brief @userdesc Set the frame multiple
             @par Python Usage:
             @code trick.var_set_frame_multiple(<mult>) @endcode
             @param mult - The requested multiple
             @return 0
            */
            int var_set_frame_multiple(unsigned int mult) ;

            /**
             @brief @userdesc Set the frame offset
             @par Python Usage:
             @code trick.var_set_frame_offset(<offset>) @endcode
             @param offset - The requested offset
             @return 0
            */
            int var_set_frame_offset(unsigned int offset) ;

            /**
             @brief @userdesc Set the frame multiple
             @par Python Usage:
             @code trick.var_set_freeze_frame_multiple(<mult>) @endcode
             @param mult - The requested multiple
             @return 0
            */
            int var_set_freeze_frame_multiple(unsigned int mult) ;

            /**
             @brief @userdesc Set the frame offset
             @par Python Usage:
             @code trick.var_set_freeze_frame_offset(<offset>) @endcode
             @param offset - The requested offset
             @return 0
            */
            int var_set_freeze_frame_offset(unsigned int offset) ;

            /**
             @brief @userdesc Command to instruct the variable server to byteswap the return values
             (only has an effect in var_binary mode).
             The default is no byteswap - it is assumed server and client are same endianness.
             If not, the user must issue the var_byteswap command.
             @par Python Usage:
             @code trick.var_byteswap(<on_off>) @endcode
             @param on_off - true (or 1) to byteswap the return data, false (or 0) to return data as is
             @return always 0
            */
            int var_byteswap(bool on_off) ;

            /**
             @brief CURRENTLY NOT IMPLEMENTED
            */
            int var_signal() ;

            /**
             @brief @userdesc Command to turn on variable server logged messages to a playback file.
             All messages received from all clients will be saved to file named "playback" in the RUN directory.
             @par Python Usage:
             @code trick.set_log_on() @endcode
             @return always 0
            */
            int set_log_on() ;

            /**
             @brief @userdesc Command to turn off variable server logged messages to a playback file.
             @par Python Usage:
             @code trick.set_log_off() @endcode
             @return always 0
            */
            int set_log_off() ;

            /**
             @brief CURRENTLY NOT IMPLEMENTED\n
             Command to instruct the variable server to send values via multicast socket.
            */
            int var_multicast(bool on_off) ;

            /**
             @brief Command to send the number of items in the var_add list.
             The variable server sends a message indicator of "3", followed by the total number of variables being sent.
            */
            int send_list_size();

            /**
             @brief Special command to instruct the variable server to send the contents of the S_sie.resource file; used by TV.
             The variable server sends a message indicator of "2", followed by a tab, followed by the file contents
             which are then sent as sequential ASCII messages with a maximum size of 4096 bytes each.
            */
            int send_sie_resource();

            /**
             @brief Special command to only send the class sie class information
            */
            int send_sie_class();

            /**
             @brief Special command to only send the enumeration sie class information
            */
            int send_sie_enum();

            /**
             @brief Special command to only send the top level objects sie class information
            */
            int send_sie_top_level_objects();

            /**
             @brief Special command to send an arbitrary file through the variable server.
            */
            int send_file(std::string file_name);

            /**
             @brief Copy client variable values from Trick memory to each variable's output buffer.
            */
            int copy_sim_data();

            /**
             @brief Write data in the appropriate format (var_ascii or var_binary) from variable output buffers to socket.
            */
            int write_data();

            /**
             @brief gets the send_stdio flag.
            */
            bool get_send_stdio() ;

            /**
             @brief sets the send_stdio flag.
            */
            int set_send_stdio(bool on_off) ;

            VariableServer * get_vs() ;
            TCDevice & get_connection() ;

            /**
             @brief Internal function used by input processor to send stdout and stderr to the client.
             @return always 0
            */
            int write_stdio(int stream , std::string text ) ;

            int freeze_init() ;

            int copy_data_freeze() ;
            int copy_data_freeze_scheduled(long long curr_tics) ;
            int copy_data_scheduled(long long curr_tics) ;
            int copy_data_top() ;

            void preload_checkpoint() ;
            void restart() ;

            long long get_next_tics() ;
            long long get_freeze_next_tics() ;

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

        protected:

            /**
             @brief Called by send_sie commands to transmit files through the socket.
            */
            int transmit_file(std::string file_name);

            /**
             @brief Called by write_data to write data to socket in var_binary format.
            */
            int write_binary_data( int Start, char *buf1, int PacketNum );

            /**
             @brief Make a time reference.
             */
            REF2* make_time_ref();

            /**
             @brief Make a "bad-reference" reference.
             */
            REF2* make_error_ref(std::string in_name);

            /** The Master variable server object. */
            static VariableServer * vs ;

            /** The listen device from the variable server\n */
            TCDevice * listen_dev;          /**<  trick_io(**) */

            /** The trickcomm device used for the connection to the client.\n */
            TCDevice connection ;            /**<  trick_io(**) */

            /** The type of connection we have.\n */
            ConnectionType conn_type ;      /**<  trick_io(**) */

            /** Value (1,2,or 3) that causes the variable server to output increasing amounts of debug information.\n */
            int debug ;                      /**<  trick_io(**) */

            /** Toggle to enable/disable this variable server thread.\n */
            bool enabled ;                   /**<  trick_io(**) */

            /** Toggle to turn on/off variable server logged messages to a playback file.\n */
            bool log ;                       /**< trick_io(**)  */

            /** Toggle to indicate var_pause commanded.\n */
            bool pause_cmd ;                 /**<  trick_io(**) */

            /** Save pause state while reloading a checkpoint.\n */
            bool saved_pause_cmd ;           /**<  trick_io(**) */

            /** Toggle to indicate var_exit commanded.\n */
            bool exit_cmd ;                  /**<  trick_io(**) */

            /** Set to true to validate all addresses before copying.\n */
            bool validate_address ;          /**<  trick_io(**) */

            /** The mutex to protect variable output buffers when copying variable values to them from Trick memory.\n */
            pthread_mutex_t copy_mutex ;     /**<  trick_io(**) */

            /** The mutex pauses all processing during checkpoint restart */
            pthread_mutex_t restart_pause ;     /**<  trick_io(**) */

            /** Dummy integer for bad references.\n */
            static int bad_ref_int ;         /**<  trick_io(**) */

            /** Dummy integer for bad references. If a variable points here, do not try to re-resolve address\n */
            static int do_not_resolve_bad_ref_int ;  /**<  trick_io(**) */

            /** The simulation time converted to seconds\n */
            double time ;                    /**<  trick_units(s) */

            /** List of client requested variables.\n */
            std::vector <VariableReference *> vars;  /**<  trick_io(**) */

            /** Toggle to set variable server copy as top_of_frame, scheduled, async \n */
            VS_COPY_MODE copy_mode ;         /**<  trick_io(**) */

            /** Toggle to set variable server writes as when copied or async.\n */
            VS_WRITE_MODE write_mode ;       /**<  trick_io(**) */

            /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            int frame_multiple ;             /**<  trick_io(**) */

            /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            int frame_offset ;               /**<  trick_io(**) */

            /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            int freeze_frame_multiple ;      /**<  trick_io(**) */

            /** multiples of frame_count to copy data.  Only used at top_of_frame\n */
            int freeze_frame_offset ;        /**<  trick_io(**) */

            /** Toggle to tell variable server to byteswap returned values.\n */
            bool byteswap ;                  /**<  trick_io(**) */

            /** Toggle to tell variable server return data in binary format.\n */
            bool binary_data ;               /**<  trick_io(**) */

            /** Toggle to tell variable server return data in binary format without the variable names.\n */
            bool binary_data_nonames ;       /**<  trick_io(**) */

            /** Toggle to tell variable server to send data multicast or point to point.\n */
            bool multicast ;                 /**<  trick_io(**) */

            /** Flag to indicate the connection has been made\n */
            bool connection_accepted ;       /**<  trick_io(**) */

            /** Value set in var_cycle command.\n */
            double update_rate ;             /**<  trick_io(**) */

            /** The update rate in integer tics.\n */
            long long cycle_tics ;           /**<  trick_io(**) */

            /** The next call time in integer tics of the job to copy client data (sync mode).\n */
            long long next_tics ;            /**<  trick_io(**) */

            /** The next call time in integer tics of the job to copy client data (sync mode).\n */
            long long freeze_next_tics ;     /**<  trick_io(**) */

            /** Indicate whether variable data has been written into buffer_in.\n */
            bool var_data_staged; /**<  trick_io(**) */

            /** number of packets copied to client \n */
            unsigned int packets_copied ; /**< trick_io(**) */

            /** Toggle to indicate sending python stdout and stderr to client\n */
            bool send_stdio ;                /**<  trick_io(**) */

            /** Holding area for the incoming message\n */
            char *incoming_msg;           /**< trick_io(**) */

            /** Message with '\r' characters removed\n */
            char *stripped_msg;           /**<  trick_io(**) */

            /** Maximum size of incoming message\n */
            static const unsigned int MAX_CMD_LEN = 200000 ;
    } ;
}

#endif

