/*
PURPOSE:
    ( RealtimeSync base class )
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef REALTIMESYNC_HH
#define REALTIMESYNC_HH

#include "trick/Clock.hh"
#include "trick/Timer.hh"

namespace Trick {

    /**
     * This class is a virtual base class for maintaining real-time synchronization
     * in a Trick simulation.  The base class provides entry names for a real-time clock
     * which derivative classes must fill in.  Time values are based in tics where
     * there are 1000000 tics/second.
     *
     * @author Robert W. Bailey
     * @author Many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */

    class RealtimeSync {

        public:

            /** Set to true to enable real-time synchronization at end of current frame.\n */
            bool enable_flag ;                    /**< trick_units(--) */

            /** Set to true to disable real-time synchronization at end of current frame.\n */
            bool disable_flag ;                   /**< trick_units(--) */

            /** Set to true when enabled, meaning real-time synchronization is currently active.\n */
            bool active ;                         /**< trick_units(--) */

            /** Length of freeze frame.\n */
            long long freeze_frame ;          /**< trick_units(s) */

            /** Time during freeze mode.  Starts with Executive::time_tics when entering freeze.\n */
            long long freeze_time_tics ;          /**< trick_units(s) */

            /** Number of consecutive overrun frames.\n */
            unsigned int frame_overrun_cnt ;      /**< trick_units(--) */

            /** Total number of overrun frames.\n */
            unsigned int total_overrun ;          /**< trick_units(--) */

            /** Maximum number of consecutive overrun conditions before shutdown.\n */
            unsigned int rt_max_overrun_cnt;      /**< trick_units(--) */

            /** Maximum overrun time before sim shutdown.\n */
            double rt_max_overrun_time;           /**< trick_units(s) */

            /** Maximum overrun time in tics before sim shutdown.\n */
            long long rt_max_overrun_time_tics ;  /**< trick_units(--) */

            /** Enter Freeze mode on an overrun condition.\n */
            bool rt_overrun_freeze;               /**< trick_units(--) */

            /** How much time did the current frame take in tics.\n */
            long long frame_sched_time ;          /**< trick_units(--) */

            /** The magnitude of the current overrun in tics.\n */
            long long frame_overrun_time ;        /**< trick_units(--) */

            /** This is the start of the frame in wall clock time.\n */
            long long last_clock_time ;           /**< trick_units(--) */

            /** tics per second copied from executive\n */
            int tics_per_sec;                     /**< trick_units(--) */

            /** Align sim time to wall clock time.\n */
            bool align_sim_to_wall_clock ;       /**< trick_units(--) */

            /** Multiple of number of tics to sync to. 1 = 1 second. \n */
            double align_tic_mult ;        /**< trick_units(--) */

            /** The real-time clock source\n */
            Trick::Clock * rt_clock ;             /**< trick_units(--) */

            /** The default real-time clock source, usually the gettimeofday clock\n */
            Trick::Clock * default_clock ;             /**< trick_units(--) */

            /** The sleep timer allows the simulation to release the CPU and
                wake up before the start of the next frame\n */
            Trick::Timer * sleep_timer ;          /**< trick_units(--) */

            /** Maximum overrun condition exceeded.  Shutdown after leaving freeze.\n */
            bool freeze_shutdown;                 /**< trick_units(--) */

            /** The clock time when the sim started.  Used for total actual time calculation.\n */
            long long sim_start_time ;           /**< trick_units(--) */

            /** The clock time when initialization ended.  Used for total actual time calculation.\n */
            long long sim_end_init_time ;           /**< trick_units(--) */

            /** The clock time when the sim ended.  Used for total actual time calculation.\n */
            long long sim_end_time ;           /**< trick_units(--) */

            /**
             @brief This is the constructor of the RealtimeSync class.  It starts the RealtimeSync as
             disabled and sets the maximum overrun parameters to basically infinity.
             @param in_clock - incoming pointer to a Clock to use for simulation time sync
             @param in_timer - incoming pointer to a Timer to use for pausing between real-time frames
             */
            RealtimeSync( Trick::Clock * in_clock , Trick::Timer * in_timer = NULL ) ;

            /**
             @brief This is the destructor of the RealtimeSync class.
             */
            virtual ~RealtimeSync() {} ;

            /**
             @brief @userdesc Command to set enabled flag to true which turns real-time syncronization on.
             @par Python Usage:
             @code trick.real_time_enable() @endcode
             @return always 0
             */
            int enable() ;

            /**
             @brief @userdesc Command to set disabled flag to true which turns real-time syncronization off (default).
             @par Python Usage:
             @code trick.real_time_disable() @endcode
             @return always 0
             */
            int disable() ;

            /**
             @brief @userdesc Command to change the real time clock
             @par Python Usage:
             @code trick.real_time_change_clock(<clock>) @endcode
             @return always 0
             */
            int change_clock(Trick::Clock * in_clock) ;

            /**
             @brief @userdesc Gets the name of the current clock used
             @par Python Usage:
             @code trick.real_time_clock_get_name() @endcode
             @return string containg clock used.
             */
            const char * clock_get_name() ;

            /**
             @brief @userdesc Command to change the real time clock
             @par Python Usage:
             @code trick.real_time_change_timer(<sleep_timer>) @endcode
             @return always 0
             */
            int change_timer(Trick::Timer * in_sleep_timer) ;

            /**
             @brief @userdesc Command to change the real time clock ratio
             @par Python Usage:
             @code trick.real_time_set_rt_clock_ratio(<clock_ratio>) @endcode
             @return always 0
             */
            int set_rt_clock_ratio(double in_clock_ratio) ;

            /**
             @brief Starts the actual elapsed timer used on the default clock
             @return always 0
             */
            virtual void get_sim_start_time() ;

            /**
             @brief Ends the actual elapsed timer for initalization
             @return always 0
             */
            virtual void get_sim_end_init_time() ;

            /**
             @brief Ends the actual elapsed timer at shutdown.
             @return always 0
             */
            virtual void get_sim_end_time() ;

            /**
             @brief Initializes the clock and sleep timer hardware.
             Called as an initialization job in the S_define file.
             @return always 0
             */
            virtual int initialize() ;

            /**
             @brief Reinializes the real-time clock after a checkpoint reload.
             Called as an restart job in the S_define file.
             @param ref_time - simulation time
             @return always 0
             */
            virtual int restart( long long ref_time ) ;

            /**
             @brief @userdesc Start real-time clock and set to the given time. Trick will automatically call this routine
             whenever real-time is enabled by the user.
             Called as an initialization job (or as a restart job after a checkpoint reload) in the S_define file, or
             by the real-time monitor if the user enables real-time while the sim is executing.
             @par Python Usage:
             @code trick.real_time_restart(<ref_time>) @endcode
             @param ref_time - simulation time in tics
             @return always 0
             */
            virtual int start_realtime(double in_frame_time , long long ref_time ) ;

            /**
             @brief Checks for simulation overruns and takes action if overrun conditions have been met.
             Called as an end_of_frame job in the S_define file.
             @param sim_time_tics - current simulation elapsed time
             @return always 0
             */
            virtual int rt_monitor(long long sim_time_tics) ;

            /**
             @brief Resets the clock to track an incoming freeze frame period.
             Called as a freeze_init job in the S_define file.
             @param in_freeze_frame_tics - freeze frame period in tics.
             @return always 0
             */
            virtual int freeze_init(double in_freeze_frame) ;

            /**
             @brief Waits for the freeze frame period to expire,  resets clock
             to wait for another freeze frame period.
             Called as a freeze job in the S_define file.
             @return always 0
             */
            virtual int freeze_pause(double in_freeze_frame) ;

            /**
             @brief Resets clock reference time to incoming simulation time.  Will
             terminate the simulation if the clock freeze-shutdown condition is true.
             Called as an unfreeze job in the S_define file.
             @param sim_time_tics - current simulation elapsed time
             @return always 0
             */
            virtual int unfreeze(long long sim_time_tics, double software_frame) ;

            /**
             @brief Calls shutdown routines for real-time clock and sleep timer.
             Called as a shutdown job in the S_define file.
             @return always 0
             */
            virtual int shutdown() ;

            /**
             @brief Initializes the sleep timer hardware.
             @return always 0
             */
            virtual int start_sleep_timer();

    } ;

} ;

#endif
