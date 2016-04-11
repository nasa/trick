/*
    PURPOSE:
        (VariableServer)
*/

#ifndef VARIABLESERVER_HH
#define VARIABLESERVER_HH

#include <map>
#include <queue>
#include <vector>
#include <string>
#include <pthread.h>
#include "trick/tc.h"
#include "trick/reference.h"
#include "trick/JobData.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/VariableServerThread.hh"
#include "trick/VariableServerListenThread.hh"
#include "trick/ThreadBase.hh"

namespace Trick {

/**
  This class provides variable server setup.
  @author Alex Lin
 */

    class VariableServer {

        public:
            /**
             @brief Constructor.
            */
            VariableServer() ;

            /**
             @brief Destructor.
            */
            ~VariableServer() ;

            /**
             @brief Set up the listen port during default_data so it is available at the start of initialization.
            */
            int default_data() ;

            /**
             @brief Start up var_serve_init thread if the variable server is enabled.
            */
            int init() ;

            /**
             @brief Checkpoint restart job
             @return always 0
            */
            int restart() ;

            /**
             @brief Sets up freeze call times for scheduled variable server clients.
            */
            int freeze_init() ;

            /**
             @brief Shutdown the variable server
             @return always 0
            */
            int shutdown() ;

            /**
             @brief Gets next time to call copy_data job for a server/client connection in sync mode.
            */
            int get_next_sync_call_time() ;

            /**
             @brief Gets next time to call copy_data job for a server/client connection in freeze mode.
            */
            int get_next_freeze_call_time() ;

            /**
             @brief Copies client variable values at the top of the frame.
            */
            int copy_data_top() ;

            /**
             @brief The function to copy client variable values to their output buffers when in sync mode.
            */
            int copy_data_scheduled() ;

            /**
             @brief The function to copy client variable values to their output buffers when in sync mode.
            */
            int copy_data_freeze_scheduled() ;

            /**
             @brief Copies client variable values at the top of the frame.
            */
            int copy_data_freeze() ;

            /**
             @brief Adds a vst to the map.
            */
            void add_vst(pthread_t thread_id, VariableServerThread * in_vst ) ;

            /**
             @brief Get a vst mapped by thread id
             @return the VariableServerThread mapped to the thread id if found, or NULL if not found.
            */
            Trick::VariableServerThread * get_vst(pthread_t thread_id) ;

            /**
             @brief Delete a vst in the map
            */
            void delete_vst(pthread_t thread_id) ;

            /**
             @brief @userdesc Return host name from the listen device.
             @par Python Usage:
             @code <my_string> = trick.var_server_get_hostname() @endcode
            */
            const char * get_hostname() ;

            /**
             @brief Returns the default listen thread.
            */
            Trick::VariableServerListenThread & get_listen_thread() ;

            /**
             @brief @userdesc Enable (default) or disable the variable server.
             @par Python Usage:
             @code trick.var_server_set_enabled(<on_off>) @endcode
             @param on_off - 1 to enable variable server, 0 to disable
             @note Disabling the variable server will disable the simulation control panel, TV, MTV, and stripchartting.
             @return always 0
            */
            void set_enabled(bool on_off) ;

            /**
             @brief @userdesc Test if the variable server is enabled.
             @par Python Usage:
             @code <my_bool> = trick.var_server_get_enabled() @endcode
             @return 1 if enabled, 0 if disabled
            */
            bool get_enabled() ;

            /**
             @brief @userdesc Test if the variable server info messaging is on.
            */
            bool get_info_msg() ;

            /**
             @brief @userdesc Test if the variable server logging is on.
            */
            bool get_log() ;

            /**
             @brief @userdesc Command to turn on variable server info messages (e.g., get a message
              when a command is received from any client).
             Variable Server info messages will be published using the Trick status message system.
             @note If you want to only see info messages for a specific client, you should instead
              send "trick.var_debug(1);" from that client.
             @par Python Usage:
             @code trick.set_var_server_info_msg_on() @endcode
             @return always 0
            */
            void set_var_server_info_msg_on() ;

            /**
             @brief @userdesc Command to turn off variable server event info messages (off is the default).
             No Variable Server info messages will be published using the Trick status message system.
             @par Python Usage:
             @code trick.set_var_server_info_msg_off() @endcode
             @return always 0
            */
            void set_var_server_info_msg_off() ;

            /**
             @brief @userdesc Command to turn on variable server logging for all clients.
             Variable Server log messages will be saved to a varserver_log file in the RUN directory.
             @par Python Usage:
             @code trick.set_var_server_log_on() @endcode
             @return always 0
            */
            void set_var_server_log_on() ;

            /**
             @brief @userdesc Command to turn off variable server logging for all clients.
             No Variable Server log messages will be saved to a varserver_log file in the RUN directory.
             @par Python Usage:
             @code trick.set_var_server_log_off() @endcode
             @return always 0
            */
            void set_var_server_log_off() ;

            /**
             @brief @userdesc Command to open additional variable server listen port.
             @param source_address - the name or numeric IP of the machine to bind listen socket.  NULL or empty
                                     string will default to default name of machine
             @param port - the port.
             @par Python Usage:
             @code trick.var_server_create_tcp_socket(source_address, port) @endcode
             @return 0 if successful
            */
            int create_tcp_socket(const char * source_address, unsigned short port ) ;

            /**
             @brief @userdesc Command to open a udp socket suitable for a single connection
             @param source_address - the name or numeric IP of the machine to bind listen socket.  NULL or empty
                                     string will default to default name of machine
             @param port - the port.
             @par Python Usage:
             @code trick.var_server_create_udp_socket(source_address, port) @endcode
             @return 0 if successful
            */
            int create_udp_socket(const char * source_address, unsigned short port ) ;

            /**
             @brief @userdesc Command to open a multicast socket suitable for a single connection
             @param mcast_address - the name or numeric IP of the multicast address to use.
             @param source_address - the name or numeric IP of the machine to bind listen socket.  NULL or empty
                                     string will default to default name of machine
             @param port - the port.
             @par Python Usage:
             @code trick.var_server_create_multicast_socket(source_address, port) @endcode
             @return 0 if successful
            */
            int create_multicast_socket( const char * mcast_address,
             const char * source_address, unsigned short port ) ;

            /**
             @brief @userdesc Suspend variable server processing in preparation for checkpoint reload.
             @return 0 if successful
            */
            int suspendPreCheckpointReload();

            /**
             @brief @userdesc Resume variable server processing following a checkpoint reload.
             @return 0 if successful
            */
            int resumePostCheckpointReload();

            /**
             @brief Called from the S_define to set the copy_data_job ptr.
            */
            void set_copy_data_job( Trick::JobData * ) ;

            /**
             @brief Called from the S_define to set the copy_data_freeze_job ptr.
            */
            void set_copy_data_freeze_job( Trick::JobData * ) ;

        protected:

            /** Toggle to enable/disable the variable server.\n */
            bool enabled ;                   /**<  trick_units(--) */

            /** Toggle to turn on/off variable server info messages (e.g., commands received from all clients).\n */
            bool info_msg ;                  /**< trick_units(--)  */

            /** Toggle to turn on/off variable server logging messages, similar to info_msg except messages only go
                to a varserver_log file in the RUN directory.\n */
            bool log ;                       /**< trick_units(--)  */

            /** Default listen port thread object */
            VariableServerListenThread listen_thread ;

            /** Pointer to automatic_last job that copies requested variable values to their output buffers in sync mode.\n */
            Trick::JobData * copy_data_job ; /**< trick_io(**) trick_units(--) */

            /** Pointer to freeze_automatic job that copies requested variable values to their output buffers in sync mode.\n */
            Trick::JobData * copy_data_freeze_job ; /**< trick_io(**) trick_units(--) */

            /** Storage for saved thread pause state. The pause state of each thread is saved
              to this map by suspendPreCheckpointReload(). resumePostCheckpointReload() restores
              the pause state from this map.
             */
            std::map<pthread_t, bool> thread_pause_state_store; // ** ignore this

            /** Map thread id to the VariableServerThread object.\n */
            std::map < pthread_t , VariableServerThread * > var_server_threads ; /**<  trick_io(**) */

            /** Mutex to ensure only one thread manipulates the map of var_server_threads\n */
            pthread_mutex_t map_mutex ;     /**<  trick_io(**) */

            /** Map of additional listen threads created by create_tcp_socket.\n */
            std::map < pthread_t , VariableServerListenThread * > additional_listen_threads ; /**<  trick_io(**) */


    } ;

}

int vs_format_ascii(Trick::VariableReference * var, char *value);

Trick::VariableServer * var_server_get_var_server() ;

// external calls to be made available from input processor
int var_add(std::string in_name) ;
int var_add(std::string in_name, std::string units_name) ;
int var_remove(std::string in_name) ;
int var_units(std::string var_name , std::string units_name) ;
int var_exists(std::string in_name) ;
int var_send() ;
int var_clear() ;
int var_cycle(double in_rate) ;
int var_pause() ;
int var_unpause() ;
int var_exit() ;
int var_write_stdio(int stream , std::string text) ;
int var_set_client_tag( std::string text) ;

int var_debug(int level) ;
int var_ascii() ;
int var_binary() ;
int var_binary_nonames() ;
int var_validate_address(int on_off) ;
int var_set_copy_mode(int mode) ;
int var_set_write_mode(int mode) ;
int var_set_send_stdio(int mode) ;
int var_sync(int mode) ;
int var_set_frame_multiple(unsigned int mult) ;
int var_set_frame_offset(unsigned int offset) ;
int var_set_freeze_frame_multiple(unsigned int mult) ;
int var_set_freeze_frame_offset(unsigned int offset) ;
int var_byteswap(bool on_off) ;

int var_signal() ;
int var_multicast(bool on_off) ;

int var_send_list_size() ;

int send_sie_resource() ;
int send_sie_class() ;
int send_sie_enum() ;
int send_sie_top_level_objects() ;
int send_file(std::string file_name) ;

int var_set( const char * var , double value , const char * units = NULL ) ;
int var_set( const char * var , long long value , const char * units = NULL ) ;
int var_set( const char * var , const char * value , const char * units = NULL ) ;
int var_set( const char * var , void * value , const char * units = NULL ) ;

int var_server_log_on() ;
int var_server_log_off() ;

#endif

