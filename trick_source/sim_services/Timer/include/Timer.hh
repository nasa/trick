/*
PURPOSE:
    ( Timer base class
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef TIMER_HH
#define TIMER_HH

namespace Trick {

    /**
     * This class is a virtual base class for maintaining real time synchronization
     * in a Trick simulation.  The base class provides entry names for a realtime timer
     * which derivative classes must fill in.  Time values are based in tics where
     * there are 1000000 tics/second
     */

    class Timer {

        public:

            Timer() ;
            virtual ~Timer() {} ;

            /**
             @brief @userdesc Command to enable using the timer during an underrun when running real-time.
             @par Python Usage:
             @code trick.itimer_enable() @endcode
             @return always 0
            */
            int enable() ;

            /**
             @brief @userdesc Command to disable using the timer during an underrun when running real-time.
             This is the default. See enable() for details.
             @par Python Usage:
             @code trick.itimer_disable() @endcode
             @return always 0
            */
            int disable() ;

            /**
             @brief @userdesc Get the status of the Timer
             @par Python Usage:
             @code trick.get_enabled() @endcode
             @return always 0
            */
            bool get_enabled() ;

            /**
             @brief @userdesc Sets the active flag
             @par Python Usage:
             @code trick.set_active(<on_off>) @endcode
             @return always 0
            */
            void set_active( bool in_active) ;

            /**
             @brief Initializes the timer.  Timer hardware (if any) should be initialized here.
             the timer's frequency is set to the incoming in_frame_time.
             @return 0 if successful, otherwise an error code.
             */
            virtual int init() = 0 ;

            /**
             @brief Starts the timer.
             */
            virtual int start(double frame_time) = 0 ;

            /**
             @brief Starts the timer after it has elapsed.
             */
            virtual int reset(double frame_time) = 0 ;

            /**
             @brief Turns the timer off.
             */
            virtual int stop() = 0 ;

            /**
             @brief Blocks until timer elapses.
             */
            virtual int pause() = 0 ;

            /**
             @brief Shutdown class job to stop timer and clean up any timer data structures.
             */
            virtual int shutdown() = 0 ;

        protected:

            /** True if the timer is enabled.\n */
            bool enabled ;           /**< trick_units(--) */

            /** True if the timer is active for this frame.\n */
            bool active ;            /**< trick_units(--) */

    } ;

} ;

#endif
