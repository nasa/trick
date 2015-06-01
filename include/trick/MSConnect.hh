/*
PURPOSE:
    (For master/slave sim, this is slave info)
*/

#ifndef MSCONNECT_HH
#define MSCONNECT_HH

#include <string>

#include "trick/ms_sim_mode.h"

namespace Trick {

    /**
     * This class is the base class of the connection between master and slaves.
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

    class MSConnect {

        public:

            /**
             @brief Empty constructor.
             */
            MSConnect() {} ;
            virtual ~MSConnect() {} ;

            /**
             @brief Sets the wait time limit for communications between the master and slaves.
             @param in_limit - the desired wait limit.
             @return always 0
             */
            virtual int set_sync_wait_limit(double in_limit) = 0 ;

            /**
             @brief Creates command line parameters specific to starting this particular connection type.
             @return string of command line parameters to add to slave startup command
             */
            virtual std::string add_sim_args(std::string slave_type) = 0 ;

            /**
             @brief Searches the command line parameters for connection specific parameters.
             @return 1 if connection parameters found, 0 if no parameters found
             */
            virtual int process_sim_args() = 0 ;

            /**
             @brief Establishes the connection on the master side of the connection.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int accept() = 0 ;

            /**
             @brief Establishes the connection on the slave side of the connection.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int connect() = 0 ;

            /**
             @brief Closes the connection on the slave side of the connection.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int disconnect() = 0;

            /**
             @brief Read the time from the other simulation.
             @return the time read or MS_ERROR_TIME if the read failed
             */
            virtual long long read_time() = 0 ;

            /**
             @brief Read a port number from the other simulation.
             @return the port read or MS_ERROR_PORT if the read failed
             */
            virtual int read_port() = 0 ;

            /**
             @brief Read a character array from the other simulation.
             @return the 1st character read or MS_ERROR_NAME if the read failed
             */
            virtual char read_name(char * read_data, size_t size) = 0 ;

            /**
             @brief Read the mode command from the other simulation.
             @return the simulation command or ErrorCmd if the read failed
             */
            virtual MS_SIM_COMMAND read_command() = 0 ;

            /**
             @brief Writes the time to the other simulation.
             @return the number of bytes written
             */
            virtual int write_time(long long sim_time) = 0 ;

            /**
             @brief Writes a port number to the other simulation.
             @return the number of bytes written
             */
            virtual int write_port(int port) = 0 ;

            /**
             @brief Writes a character array from in_data to the other simulation.
             @return the number of bytes written
             */
            virtual int write_name(char * in_data, size_t size) = 0 ;

            /**
             @brief Writes the mode command to the other simulation.
             @return the number of bytes written
             */
            virtual int write_command(MS_SIM_COMMAND command) = 0 ;

            /** Limit of how long to wait for a message.\n */
            double sync_wait_limit ;  /**< trick_units(s) */
    } ;
}

#endif
