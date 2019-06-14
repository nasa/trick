/*
    PURPOSE:
        (Trick input processor.)
    REFERENCE:
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment)
          (NASA:JSC #37943)
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--)))
    ASSUMPTIONS AND LIMITATIONS:
        ()
    PROGRAMMERS:
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime))
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

#ifndef IPPYTHON_HH
#define IPPYTHON_HH

#include <string>
#include <pthread.h>
#include "trick/InputProcessor.hh"

namespace Trick {

/**
  This class provides Python input processing.
  @author Alex Lin, Danny Strauss
 */

    class IPPython : public InputProcessor {

        friend class IPPythonEvent;

        public:

            /** Returned value from event condition evaluation.\n */
            int return_val ;                              /**< trick_io(**) trick_units(--) */

            /** Input processor mutex for protection for var server and event processing.\n */
            pthread_mutex_t ip_mutex;                     /**< trick_io(**) trick_units(--) */

            /**
             @brief Constructor.
            */
            IPPython() ;

            /**
             @brief Creates python handles for all memory manager named variables.
            */
            void get_TMM_named_variables() ;

            /**
             @brief Get the status of the units conversion message flag
            */
            bool get_units_conversion_msgs() ;

            /**
             @brief Setting to see units conversions warnings
            */
            void shoot_the_units_conversion_messenger(bool onoff) ;

            /**
             @brief Initialize and run the Python input processor on the user input file.
            */
            virtual int init() ;

            /**
             @brief @userdesc Command to shutdown the simulation now.
             @par Python Usage:
             @code trick.shutdown() @endcode
            */
            virtual int shutdown() ;

            /**
             @brief Command to parse the given string.
            */
            virtual int parse(std::string in_string) ;

            /**
             @brief Command to parse the given string as a condition statement.
            */
            virtual int parse_condition(std::string in_string, int & cond_return_val) ;

            /**
             @brief Restore variables with memory manager names to python space.
             @return always 0
            */
            int restart() ;

        protected :
            /** TODO: remove units_conversion_msgs in 2021 */
            /** false = see units conversion messages, true = head in sand */
            bool units_conversion_msgs ;

    } ;

}

extern "C" {
//SWIG generated routine.
void init_swig_modules(void) ;

/* Call this if you you refuse to fix your units problems and want to shoot the messenger instead */
void shoot_the_units_conversion_messenger() ;
void revive_the_units_conversion_messenger() ;

int check_units_conversion_messenger_for_signs_of_life() ;
}

#endif

