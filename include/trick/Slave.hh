/*
    PURPOSE:
        (Slave object for master/slave synchronization)
*/

#ifndef SLAVE_HH
#define SLAVE_HH

#include "trick/MSConnect.hh"

namespace Trick {

    /**
     * This class is the slave of a master/slave connection.  The slave
     * communicates with the master at the end of frame and mode transitions.
     *
     * @author Robert W. Bailey from 1991-1992
     * @author Many other Trick developers of the past
     * @author Alexander S. Lin, refactor into c++ classes
     *
     * @date Feb. 2010
     *
     */

    class Slave {

        public:
            /**
             @brief Constructor that sets the slave enabled flag to false.
             */
            Slave() ;

            /* True when this slave is activated (default is false).\n */
            bool activated ;                  /**< trick_units(--) */

            /* True when this slave is enabled (default is false), see process_sim_args.\n */
            bool enabled ;                    /**< trick_units(--) */

            /* True when this slave has reconnected (dmtcp_restart) */
            bool reconnected ;                /**< trick_units(--) */

            /* True when this slave has sent ReconnectCmd to master (default is false).\n */
            bool sent_reconnect_cmd;          /**< trick_units(--) */

            /* True when this slave has published a message that it has reconnected to master(default is false).\n */
            bool msg_published;               /**< trick_units(--) */

            /** @userdesc Read in master's checkpoint file name here, used for forming slave's checkpoint file name.\n
                 Also used to send slave's dmtcp checkpoint file name to master when loading dmtcp checkpoint.\n **/
            char chkpnt_name[256];            /**< trick_units(--) */

            /** @userdesc True means terminate the slave if it loses synchronization with the master.\n
                False (default) means freeze the slave if it loses synchronization with the master,
                      in which case the master will also freeze.\n
                A loss of synchronization occurs when communication takes longer than sync_wait_limit.\n */
            bool sync_error_terminate;        /**< trick_units(--) */

            /** The time period (in seconds) that the slave's connection will wait to read sync data
              from the master (and vice versa); this value is set by reading it from the master.\n
              The default is 0.0, which to an MSSocket connection means infinite (non-blocking).\n */
            double sync_wait_limit ;         /**< trick_units(s) */

            /** Connection to the master.\n */
            Trick::MSConnect * connection ;   /**< trick_io(**) trick_units(--) */

            /**
             @brief @userdesc Command to set the slave's connection type to the master.  Each slave may have a different connection type.
             @par Python Usage:
             @code <master_slave_sim_obj>.<slave_obj>.set_connection_type(<in_connection>) @endcode
             @param in_connection - A Trick::MSConnect derived class pointer (like MSSocket) that defines connection type to use
             @return always 0
             */
            int set_connection_type(Trick::MSConnect * in_connection) ;

            /**
             @brief Handles command line arguments specific to the slave, and enables slave if the "-p" arg is found.
             @return always 0
             */
            int process_sim_args() ;

            /**
             @brief Initializes master/slave communications by connecting to the master.  Disables this simulation
             of being a master, and disables the RealtimeSync object if it is present.
             @return always 0
             */
            int init() ;

            /**
             @brief Reconnect master/slave communications for dmtcp restart.
             @return always 0
             */
            int dmtcp_restart() ;

            /**
             @brief Read the master's checkpoint name.
             @return the appropriate name to use for the current dump/load command
             */
            std::string get_checkpoint_name(MS_SIM_COMMAND command) ;

            /**
             @brief End of frame job that performs synchronization with the master.
             Writes the slave's mode command to the master.
             Reads the master simulation time and mode command from the master.
             @return always 0
             */
            int end_of_frame() ;

            /**
             @brief Sets the synchronization timeout value to infinite because while in freeze mode,
             freeze frame periods may vary.
             @return always 0
             */
            int freeze_init() ;

            /**
             @brief Synchronizes the slave in freeze mode by calling the end of frame routine.
             @return always 0
             */
            int freeze() ;

            /**
             @brief Resets the synchronization wait time to the default value.
             @return always 0
             */
            int unfreeze() ;

            /**
             @brief Tells the master this slave is shutting down.
             @return always 0
             */
            int shutdown() ;

    } ;

}

#endif

