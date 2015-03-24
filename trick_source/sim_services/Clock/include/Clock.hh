/*
PURPOSE:
    ( Clock base class )
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef CLOCK_HH
#define CLOCK_HH

#include <string>

namespace Trick {

    /**
     * This class is a virtual base class for maintaining real time synchronization
     * in a Trick simulation.  The base class provides entry names for a realtime clock
     * which derivative classes must fill in.
     *
     * @author Robert W. Bailey
     * @author Many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */

    class Clock {

        public:

            /**
             * This is the constructor of the Clock class.  It starts the Clock as
             * disabled and sets the maximum overrun parameters to basically infinity.
             * @param in_clock_tics_per_sec - number of tics per second of the hardware clock
             * @param in_name - name of the clock to use in printouts.
             */
            Clock( unsigned long long in_clock_tics_per_sec , std::string in_name ) ;

            /**
             * This is the destructor of the Clock class.
             */
            virtual ~Clock() {} ;

            /**
             * @brief Gets the simulation time tics and calculates sim time to clock time tic ratio.
             * @return 0
             */
            virtual int calc_sim_time_ratio( int in_time_tic_value ) ;

            /**
             * @brief Gets the rt_clock_ratio
             * @return current clock ratio
             */
            double get_rt_clock_ratio() ;

            /**
             * @brief Sets the global_clock_pointer to this instance
             * @return returns 0
             */
            int set_global_clock() ;

            /**
             * @brief Sets the rt_clock_ratio
             * @return returns 0 if clock_ratio sucessfully set.
             */
            int set_rt_clock_ratio(double) ;

            /**
             * @brief Moves the ref_time_tics by the number of incoming simulation time tics
             * @return 0
             */
            int adjust_ref_time(long long in_tics) ;

            /**
             * @brief Sets the reference time to the incoming value
             * @param ref - the reference time in tics
             * @return always 0
             */
            int set_reference(long long ref) ;

            /**
             * @brief Syncs clock to next whole second
             * @return 0
             */
            virtual int sync_to_wall_clock(double align_tic_mult, int tics_per_sec) ;

            /**
             * @brief Gets the simulation elapsed time from the clock.
             */
            virtual long long clock_time() ;

            /**
             * @brief Resets the clock's reference time is set to be relative to the incoming ref
             * @param ref - the simulation time in tics
             * @return current wall clock time in tics
             */
            virtual long long clock_reset(long long ref) ;

            /**
             * @brief Gets the name of the clock
             * @return string with the clock name
             */
            virtual const char * get_name() ;

            /**
             * @brief Initializes the clock.  Clock hardware (if any) should be initialized here.
             * @param ref - the simulation time in tics
             * @return return 0 if successful. Otherwise return error code.
             */
            virtual int clock_init() = 0 ;

            /**
             * @brief Gets the actual time from the clock.
             */
            virtual long long wall_clock_time() = 0 ;

            /**
             * @brief Enters a spin loop waiting for wall clock time to match the requested simulation time
             * @param req_time - the simulation time in tics
             * @return current wall clock time in tics
             */
            virtual long long clock_spin(long long req_time) ;

            /**
             * @brief Stops the real time clock
             * @return always 0
             */
            virtual int clock_stop() = 0 ;

            // These variables are left public to allow the variable server access

            /** Name of the clock\n */
            std::string name ;

            /** Number of tics per second of the hardware clock\n */
            unsigned long long clock_tics_per_sec ;  /**< trick_units(--) */

            /** Ratio of desired sim time to real-time. i.e. 2.0 = sim executes 2
                times faster than realtime, 0.1 = sim executes 10 times slower than real-time.\n */
            double rt_clock_ratio ;               /**< trick_units(--) */

            /** Ratio of sim time tics to clock tics.\n */
            double sim_tic_ratio ;

            /** Reference time where the current time is relative to.\n */
            long long ref_time_tics ;             /**< trick_io(**) */

    } ;

} ;

#endif
