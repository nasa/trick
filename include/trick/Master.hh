/*
    PURPOSE:
        (The master of the master/slave connection.)
    REFERENCE:
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment)
          (NASA:JSC #37943)
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--)))
    PROGRAMMERS:
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime))
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

#ifndef MASTER_HH
#define MASTER_HH

#include <vector>
#include <queue>
#include <set>
#include "trick/MSConnect.hh"
#include "trick/RemoteShell.hh"
#include "trick/ms_sim_mode.h"

#ifdef SWIG
// This allows SWIG access to the inside of the slave vector
%template(slaveVector) std::vector<Trick::SlaveInfo *> ;
#endif

namespace Trick {

    /**
     * This class keeps the remote execution information for each slave simulation controlled by the
     * master.  It also contains the actual conneciton structure to each slave simulation.
     *
     * @author Robert W. Bailey from 1991-1992
     * @author Many other Trick developers of the past
     * @author Alexander S. Lin, refactor into c++ classes
     *
     * @date Feb. 2010
     *
     */

    class SlaveInfo {

        public:

            /**
             @brief @userdesc Tell the master about a slave by constructing a new slaveinfo object.
             @par Python Usage:
             @code <new_slave> = trick.SlaveInfo() @endcode
             @return the new SlaveInfo object
            */
            SlaveInfo() ;

            /** Is this slave enabled ? (default is true).\n */
            bool enabled ;                   /**< trick_units(--) */

            /** True when this slave has been activated by the master.\n
             May become false if the master loses sync with the slave,
             in which case the master will no longer communicate with the slave.\n */
            bool activated ;                 /**< trick_units(--) */

            /** Indicates "dmtcp" or "ascii" slave. Used to contruct sync_port_tag (default is "undefined").\n*/
            std::string slave_type;          /**< trick_units(--) */

            /** Slave's dmtcp port if slave_type "dmtcp" (=0 if slave_type "ascii").\n*/
            long long dmtcp_port;            /**< trick_units(--) */

            /** @userdesc Which remote shell shall the master use to start the slave.\n
             TRICK_SSH means use ssh (the default), TRICK_RSH means use rsh, TRICK_USER_REMOTE_SH means use custom.\n */
            Trick::RemoteShell remote_shell; /**< trick_units(--) */

            /** @userdesc User defined custom remote shell command, only used when remote_shell == TRICK_USER_REMOTE_SH.\n */
            std::string user_remote_shell ;       /**< trick_units(--) */

            /** @userdesc Optional arguments to use with the remote shell command (appended after shell command).\n */
            std::string remote_shell_args ;       /**< trick_units(--) */

            /** @userdesc Machine name to run slave on (default is "localhost").\n */
            std::string machine_name;             /**< trick_units(--) */

            /** @userdesc Machine to send slave X display (default uses the DISPLAY environment variable).\n */
            std::string machine_display;          /**< trick_units(--) */

            /** @userdesc Path to slave simulation directory, which will be used in a "cd" command
              to change to that directory before calling S_main_name.\n */
            std::string sim_path;                 /**< trick_units(--) */

            /** @userdesc Slave executable name, default is "./S_main_${TRICK_HOST_CPU}.exe".\n */
            std::string S_main_name;              /**< trick_units(--) */

            /** @userdesc When master dumps a checkpoint, command the slave to dump a checkpoint (default=true).\n */
            bool chkpnt_dump_auto;                /**< trick_units(--) */

            /** @userdesc When master loads a checkpoint, command the slave to load a checkpoint (default=true).\n
                If chkpnt_binary is true, the slave will terminate and the master will load the slave's dmtcp checkpoint.\n */
            bool chkpnt_load_auto;                /**< trick_units(--) */

            /** @userdesc When master dumps/loads a checkpoint, this indicates the format of the slave checkpoint.\n
                The default = false which means the typical trick ascii checkpoint. True means a dmtcp checkpoint.\n */
            bool chkpnt_binary;                   /**< trick_units(--) */

            /** @userdesc Send master's checkpoint file name to slave from here.\n
                 Also used to read dmtcp checkpoint file name from slave when loading dmtcp checkpoint.\n **/
            char chkpnt_name[256];                /**< trick_units(--) */

            /** @userdesc The "RUN_<dir>/<input_file>" of the slave to use as the parameter to S_main_name.\n */
            std::string run_input_file;           /**< trick_units(--) */

            /** @userdesc Other user arguments to send to the slave simulation, appended using a "-u".\n */
            std::string other_args;               /**< trick_units(--) */

            /** @userdesc True means terminate the master if it loses synchronization with this slave.\n
                False (default) means deactivate the slave if master loses synchronization with this slave,
                      in which case the master will no longer attempt communication with the slave.\n
                A loss of synchronization occurs when communication takes longer than sync_wait_limit
                or when this slave terminates before the master terminates.\n */
            bool sync_error_terminate;       /**< trick_units(--) */

            /** @userdesc The time period (in seconds) that the master's connection will wait to read sync data
              from the slave and vice versa.\n
              The default is 0.0, which to an MSSocket connection means infinite (non-blocking).\n */
            double sync_wait_limit ;         /**< trick_units(s) */

            /** @userdesc How long the master will wait for this slave to reconnect if it has exited.\n */
            double reconnect_wait_limit ;  /**< trick_units(s) */

            /** The current count of how many sync_wait_limit cycles we've been waiting for slave to reconnect.\n */
            int reconnect_count ;  /**< trick_io(**) trick_units(--) */

            /** Connection to the slave.\n */
            Trick::MSConnect * connection ;  /**< trick_units(--) */

            /**
             @brief @userdesc Command to set the master's connection type to this slave.  Each slave may have a different connection type.
             @par Python Usage:
             @code <new_slave>.set_connection_type(<in_connection>) @endcode
             @param in_connection - A Trick::MSConnect derived class pointer (like MSSocket) that defines connection type to use
             @return always 0
             */
            int set_connection_type(Trick::MSConnect * in_connection) ;

            /**
             @brief Creates the remote shell command and starts the slave simulation.
             @return always 0
             */
            int start() ;

            /**
             @brief End of frame job that reads the slave status.
             Reads the mode command from slave and takes action if it is freeze or exit.
             @return always 0
             */
            int read_slave_status() ;

            /**
             @brief End of frame job that writes the master commands to the slave
             Writes the master simulation time and mode command to slave.
             @return always 0
             */
            int write_master_status() ;

            /**
             @brief Writes the master checkpoint dir/filename to the slave before a checkpoint dump/load
             @return always 0
             */
            int write_master_chkpnt_name(std::string full_path_name) ;

            /**
             @brief Restart the slave's DMTCP executable after 1) killing its dmtcp_coordinator, and
             2) disconnecting and starting a new master/slave socket connection.
             @return always 0
             */
             int restart_dmtcp_slave();
    } ;

    /**
     * This class is the master of the master/slave synchronization setup.  The master tells the
     * slave what simulation time it is, when to start the frame of execution, and when to change mode.
     *
     * @author Robert W. Bailey from 1991-1992
     * @author Many other Trick developers of the past
     * @author Alexander S. Lin, refactor into c++ classes
     *
     * @date Feb. 2010
     *
     */

    class Master {

        public:

            /**
             @brief Constructor that sets master/slave synchronization enabled flag to off.
             */
            Master() ;

            /** True when master/slave synchronization is enabled (default is false).\n
              When not enabled, only the master simulation will be executed.\n */
            bool enabled ;                         /**< trick_units(--) */

            /** A count of the slaves in the vector.  This variable is required for variable server access.\n */
            int num_slaves ;  /**< trick_units(--) */

            /** Vector of slaves tracked by the master.\n */
            std::vector< Trick::SlaveInfo * > slaves ;  /**< trick_io(**) trick_units(--) */

            /**
             @brief @userdesc Command to enable the master/slave synchronization.
             @par Python Usage:
             @code <master_slave_sim_obj>.<master_obj>.enable() @endcode
             @return always 0
             */
            int enable(void) ;

            /**
             @brief @userdesc Command to disable the master/slave synchronization (run the master sim only
             with no slave execution or interaction).
             @par Python Usage:
             @code <master_slave_sim_obj>.<master_obj>.disable() @endcode
             @return always 0
             */
            int disable(void) ;

            /**
             @brief @userdesc Command to add a new slave to the master's slave vector.
             @par Python Usage:
             @code <master_slave_sim_obj>.<master_obj>.add_slave(<new_slave>) @endcode
             @param new_slave - pointer to new slave, or if NULL, add_slave will allocate a new slave
             @return pointer to new slave
             */
            Trick::SlaveInfo * add_slave( Trick::SlaveInfo * new_slave = NULL ) ;


            /**
             @brief Handles command line arguments specific to the master (currently does nothing).
             @return always 0
             */
            int process_sim_args() ;

            /**
             @brief Initializes master/slave communications by starting all of the enabled slaves.
             @return always 0
             */
            int init() ;

            /**
             @brief End of frame class job that executes a synchronization job for each enabled slave.
             Reads the slave's mode command from the slave.
             This job effectively waits for the slave to finish BEFORE rt_monitor.
             @return always 0
             */
            int end_of_frame_status_from_slave() ;

            /**
             @brief End of frame class job that executes a synchronization job for each enabled slave.
             Writes the master simulation time and mode command to the slave.
             This job effectively tells the slave to begin the next frame AFTER rt_monitor.
             @return always 0
             */
            int end_of_frame_status_to_slave() ;

            /**
             @brief Sets the synchronization timeout value of all slave connections to infinite because
             while in freeze mode, freeze frame periods may vary.
             @return always 0
             */
            int freeze_init() ;

            /**
             @brief Synchronizes the master and slaves in freeze mode by calling the end of frame routine.
             @return always 0
             */
            int freeze() ;

            /**
             @brief Tells the slave when a checkpoint dump has occurred by calling the end of frame routine.
             @return always 0
             */
            int checkpoint();

            /**
             @brief Tells the slave when a checkpoint load has occurred by calling the end of frame routine.
             @return always 0
             */
            int preload_checkpoint();

            /**
             @brief Resets the synchronization wait time to the default value.
             @return always 0
             */
            int unfreeze() ;

            /**
             @brief Tells the slaves the master is shutting down by calling the end of frame routine.
             @return always 0
             */
            int shutdown() ;
    } ;
}

#endif
