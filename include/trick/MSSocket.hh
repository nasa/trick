/*
PURPOSE:
    (For master/slave sim, this is slave info)
PROGRAMMERS:
    (((Eddie J. Paddock) (LinCom) (April 1996) (--) (Initial Version))
     ((Scott Killingsworth) (LinCom) (September 1997) (--) (--)))
*/

#ifndef MSSOCKET_HH
#define MSSOCKET_HH

#include "trick/tc.h"
#include "trick/MSConnect.hh"

namespace Trick {

    /**
     * This class is socket based MSConnect class to connect master and slaves.
     * It defines the master/slave connection calls as well as the calls used by
     * both sides to send the time and mode commands.
     *
     * @author Eddie J. Paddock (Apr 1996)
     * @author Scott Killingsworth (Sep 1997)
     * @author Many other Trick developers of the past
     * @author Alexander S. Lin, refactor into c++ classes
     *
     * @date Feb. 2010
     *
     */

    class MSSocket : public MSConnect {

        public:

            /**
             @brief @userdesc Construct a new master/slave connection that will communicate via Trickcommm TCP/IP socket.
             Initializes the TCDevice and sets default sync wait limit as infinite.
             @par Python Usage:
             @code <socket_object> = trick.MSSocket() @endcode
             @return the new MSSocket object
            */
            MSSocket() ;
            virtual ~MSSocket() {};

            /**
             @brief Sets the wait time limit for communications between the master and slaves.
             Any @c in_limit <= 0.0 means an infinite wait limit, i.e. regular non-blocking.
             Calls tc_blockio (and tc_set_blockio_timeout_limit when @c in_limit > 0).
             @param in_limit - the desired wait limit.
             @return always 0
             */
            virtual int set_sync_wait_limit(double in_limit) ;

            /**
             @brief Creates command line parameters specific to starting this particular connection type.
             Will be appended to the master's startup command for the slave after S_main_name and run_input_file.
             @return string of command line parameters to add to slave startup command
             */
            virtual std::string add_sim_args(std::string slave_type) ;

            /**
             @brief Searches the command line parameters for connection specific parameters.
             @return 1 if connection parameters found, 0 if no parameters found
             */
            virtual int process_sim_args() ;

            /**
             @brief Establishes the connection on the master side of the connection.
             Calls tc_multiconnect.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int accept() ;

            /**
             @brief Establishes the connection on the slave side of the connection.
             Calls tc_multiconnect.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int connect() ;

            /**
             @brief Closes the connection on the slave side of the connection.
             Calls tc_multiconnect.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int disconnect();

            /**
             @brief Read the simulation time from the other simulation. Calls tc_read.
             @return the time read or MS_ERROR_TIME if the read failed
             */
            virtual long long read_time() ;

            /**
             @brief Read the mode command from the other simulation. Calls tc_read.
             @return the simulation command or ErrorCmd if the read failed
             */
            virtual MS_SIM_COMMAND read_command() ;

            /**
             @brief Read a port number (i.e. dmtcp port) from the other simulation. Calls tc_read.
             @return the port read or MS_ERROR_PORT if the read failed
             */
            virtual int read_port() ;

            /**
             @brief Read a character array (i.e. chkpnt name) into read_data from the other simulation. Calls tc_read.
             @return the 1st character read or MS_ERROR_NAME if the read failed
             */
            virtual char read_name(char * read_data, size_t size) ;

            /**
             @brief Writes the simulation time to the other simulation. Calls tc_write.
             @return the number of bytes written
             */
            virtual int write_time(long long sim_time) ;

            /**
             @brief Writes the mode command to the other simulation. Calls tc_write.
             @return the number of bytes written
             */
            virtual int write_command(MS_SIM_COMMAND command) ;

            /**
             @brief Writes a port number (i.e. dmtcp port) to the other simulation. Calls tc_write.
             @return the number of bytes written
             */
            virtual int write_port(int port) ;

            /**
             @brief Writes a character array (i.e. chkpnt name) from in_data to the other simulation. Calls tc_write.
             @return the number of bytes written
             */
            virtual int write_name(char * in_data, size_t size) ;

            /** The Trickcomm socket connection between the master and slave.\n */
            TCDevice tc_dev ;        /**< trick_units(--) */

            /** An identification tag unique to the master that tc_multiconnect will use to
                connect master and slaves.\n */
            std::string sync_port_tag ;   /**< trick_units(--) */
    } ;

}

#endif
