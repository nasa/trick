/*
PURPOSE:
    (For master/slave sim, this implements the connection as shared memory)
PROGRAMMERS:
    (((Eddie J. Paddock) (LinCom) (April 1996) (--) (Initial Version))
     ((Danny Strauss) (L3) (September 2012) (--) (--)))
*/

#ifndef MSSHAREDMEM_HH
#define MSSHAREDMEM_HH

#include "trick/tsm.h"
#include "trick/MSConnect.hh"

//-----------------------------------------------------------------------------
// MACROS USED FOR IMPLEMENTING sync_wait_limit
#include "trick/release.h" // for RELEASE()
// Mac Os X does not have clock_gettime !!
#if __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#define clock_gettime(X,tp) \
    clock_serv_t cclock; \
    mach_timespec_t mts; \
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock); \
    clock_get_time(cclock, &mts); \
    mach_port_deallocate(mach_task_self(), cclock); \
    (tp)->tv_sec = mts.tv_sec; \
    (tp)->tv_nsec = mts.tv_nsec
#endif

//-----------------------------------------------------------------------------
// MACROS USED TO IMPLEMENT QUEUES FOR READ/WRITE DATA IN MSSharedMemData
#define MSQ_MAXSIZE 5
#define MSQ_DECLARE(q,type) type q[MSQ_MAXSIZE] ; /**< trick_io(**) trick_units(--) */\
                         int q##_front ;    /**< trick_io(**) trick_units(--) */\
                         int q##_back ;     /**< trick_io(**) trick_units(--) */
#define MSQ_INIT(q)      q##_front = q##_back = 0
#define MSQ_ISEMPTY(q)   (q##_front==q##_back)
#define MSQ_FRONT(q)     q[q##_front]
#define MSQ_POP(q)       ++q##_front %= (MSQ_MAXSIZE)
#define MSQ_SIZE(q)      q##_back-q##_front
#define MSQ_PUSH(q,data) q[q##_back] = data ; ++q##_back %= (MSQ_MAXSIZE)
//-----------------------------------------------------------------------------

namespace Trick {

    /**
     * This class is shared memory based MSConnect class to connect master and slaves.
     * It defines the master/slave connection calls as well as the calls used by
     * both sides to send the time and mode commands.
     *
     * @author Eddie J. Paddock (Apr 1996)
     * @author Scott Killingsworth (Sep 1997)
     * @author Many other Trick developers of the past
     * @author Danny Strauss, use shared memory instead of a socket
     *
     * @date Sep. 2012
     *
     */

    /** The data to read/write between the master and slave in shared memory.\n */
    typedef struct {
        pid_t master_pid ;                      /**< trick_units(--) */
        MSQ_DECLARE (master_time, long long)
        MSQ_DECLARE (master_command, MS_SIM_COMMAND)
        MSQ_DECLARE (slave_command, MS_SIM_COMMAND)
        // checkpoint data is not sent every frame, so dont need a queue
        int slave_port;                         /**< trick_units(--) slave's dmtcp checkpoint port */
        char chkpnt_name[256];                  /**< trick_units(--) checkpoint dir/filename */
    } MSSharedMemData;

    class MSSharedMem : public MSConnect {

        public:

            /**
             @brief @userdesc Construct a new master/slave connection that will communicate via shared memory.
             Initializes the shared memory address and sets default sync wait limit as infinite.
             @par Python Usage:
             @code <sharedmem_object> = trick.MSSharedMem() @endcode
             @return the new MSSharedMem object
            */
            MSSharedMem() ;
            //virtual ~MSSharedMem() {};
            ~MSSharedMem() ;

            /**
             @brief Sets the wait time limit for communications between the master and slaves.
             Any @c in_limit <= 0.0 means an infinite wait limit.
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
             Calls shmget and shmat.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int accept() ;

            /**
             @brief Establishes the connection on the slave side of the connection.
             Calls shmget and shmat.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int connect() ;


            /**
             @brief Closes the connection on the slave side of the connection.
             @return 0 if connection successful, or error status otherwise
             */
            virtual int disconnect() ;

            /**
             @brief Read the simulation time from the other simulation.
             @return the time read or MS_ERROR_TIME if the read failed
             */
            virtual long long read_time() ;

            /**
             @brief Read the mode command from the other simulation.
             @return the simulation command or ErrorCmd if the read failed
             */
            virtual MS_SIM_COMMAND read_command() ;

            /**
             @brief Read a port number (i.e. dmtcp port) from the other simulation.
             @return the port read or MS_ERROR_PORT if the read failed
             */
            virtual int read_port() ;

            /**
             @brief Read a character array (i.e. chkpnt name) into read_data from the other simulation.
             @return the 1st character read or MS_ERROR_NAME if the read failed
             */
            virtual char read_name(char * read_data, size_t size) ;

            /**
             @brief Writes the simulation time to the other simulation.
             @return the number of bytes written
             */
            virtual int write_time(long long sim_time) ;

            /**
             @brief Writes the mode command to the other simulation.
             @return the number of bytes written
             */
            virtual int write_command(MS_SIM_COMMAND command) ;

            /**
             @brief Writes a port number (i.e. dmtcp port) to the other simulation.
             @return the number of bytes written
             */
            virtual int write_port(int port) ;

            /**
             @brief Writes a character array (i.e. chkpnt name) from in_data to the other simulation.
             @return the number of bytes written
             */
            virtual int write_name(char * in_data, size_t size) ;

            /** Wait a short time before next read attempt, return total time waited.\n */
            double read_wait(struct timespec *start) ;

            /** The Trick shared memory device between the master and slave.\n */
            TSMDevice tsm_dev ;             /**< trick_units(--) */

            /** Address of data to read/write between the master and slave in shared memory.\n */
            MSSharedMemData * shm_addr ;    /**< trick_units(--) */
    } ;
}

#endif
