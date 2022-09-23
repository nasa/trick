#ifndef IPPYTHONEVENT_HH
#define IPPYTHONEVENT_HH
/*
    PURPOSE: ( IPPythonEvent Class provides python input file event capability.)
*/
#include <string>
#include "trick/Event.hh"
#include "trick/reference.h"
#include "trick/JobData.hh"

#ifdef SWIG
// SWIG macros are not normally run on classes in sim_services.  We can add the the processed code manually
%trick_swig_class_typemap(Trick::IPPythonEvent, Trick__IPPythonEvent)
#endif

namespace Trick {

    class IPPython ;
    class MTV ;

    /** Data associated with each event condition.\n */
    struct condition_t {

        condition_t() ;
        /** True means condition is to be evaluated during event processing.\n */
        char enabled ;                          /**< trick_io(*io) trick_units(--) */
        /** True means that when fired, condition stays fired.\n */
        char hold ;                             /**< trick_io(*io) trick_units(--) */
        /** True when the condition evaluated to true.\n */
        char fired ;                            /**< trick_io(*io) trick_units(--) */
        /** Count of how many times this condition fired.\n */
        int  fired_count ;                      /**< trick_io(*io) trick_units(--) */
        /** Last simulation time this condition fired.\n */
        double fired_time ;                     /**< trick_io(*io) trick_units(s) */
        /** The string containing condition to be evaluated.\n */
        std::string str;                        /**< trick_io(*io) trick_units(--) */
        /** Optional comment provided by user to display in mtv.\n */
        std::string comment;                    /**< trick_io(*io) trick_units(--) */
        /** Variable reference to a model variable to be used for condition.\n */
        REF2 * ref ;                            /**< trick_io(**) trick_units(--) */
        /** Job pointer to a model job to be used for condition.\n */
        Trick::JobData * job ;                  /**< trick_io(**) trick_units(--) */
        /** Type of condition string: 0=python, 1=variable, 2=job.\n */
        int  cond_type ;                        /**< trick_io(*io) trick_units(--) */
    } ;

    /** Data associated with each event action.\n */
    struct action_t {

        action_t() ;
        /** True means action is to be run when fired.\n */
        char enabled ;                          /**< trick_io(*io) trick_units(--) */
        /** True when the action was run.\n */
        char ran ;                              /**< trick_io(*io) trick_units(--) */
        /** Count how many times this action has been run.\n */
        int  ran_count ;                        /**< trick_io(*io) trick_units(--) */
        /** Last simulation time this action ran.\n */
        double ran_time ;                       /**< trick_io(*io) trick_units(s) */
        /** The string containing action to be run.\n */
        std::string str;                        /**< trick_io(*io) trick_units(--) */
        /** Optional comment provided by user to display in mtv.\n */
        std::string comment;                    /**< trick_io(*io) trick_units(--) */
        /** Job pointer to a model job to run as the action.\n */
        JobData * job ;                         /**< trick_io(**) trick_units(--) */
        /** Type of action string: 0=python, 1=job ON, 2=job OFF 3=job call.\n */
        int  act_type ;                         /**< trick_io(*io) trick_units(--) */
    } ;

/**
  This class provides python input file event capability.
  @author Alex Lin, Danny Strauss
 */

    class IPPythonEvent : public Trick::Event {

        public:

            /** True when event is in the event_list to be processed; false when it is to be removed from event_list.\n */
            bool added ;                            /**< trick_io(*io) trick_units(--) */

            /** True for a user input event, false for a read or other trick event.\n */
            bool is_user_event ;                    /**< trick_io(*io) trick_units(--) */

            /** @userdesc True when user issues a manual command, normal condition evaluation is overriden.\n */
            bool manual ;                           /**< trick_io(*io) trick_units(--) */

            /** @userdesc True when event fired because of a manual_on or manual_fire command.\n */
            bool manual_fired ;                     /**< trick_io(*io) trick_units(--) */

            /** Toggle to turn on/off event info messages (e.g., when an event fires).\n */
            static bool info_msg ;            /**< trick_io(**) trick_units(--) */

            /** True when ALL conditions must be true to make action(s) run; default is false.\n */
            bool cond_all ;                         /**< trick_io(*io) trick_units(--) */
            /** @userdesc True when event conditions setup was evaluated as true.\n */
            char fired ;                            /**< trick_io(*io) trick_units(--) */
            /** True when an event condition fired and its hold is true, also used for manual mode hold.\n */
            char hold ;                             /**< trick_io(*io) trick_units(--) */
            /** @userdesc True when any event action was run.\n */
            char ran ;                              /**< trick_io(*io) trick_units(--) */
            /** Count of how many conditions this event has.\n */
            int condition_count ;                   /**< trick_io(*io) trick_units(--) */
            /** @userdesc Count of how many times this event has fired.\n */
            int fired_count ;                       /**< trick_io(*io) trick_units(--) */
            /** @userdesc Last simulation time that this event fired.\n */
            double fired_time ;                     /**< trick_io(*io) trick_units(s) */
            /** Count of how many actions this event has.\n */
            int action_count ;                      /**< trick_io(*io) trick_units(--) */
            /** @userdesc Count of how many times this event has run its actions.\n */
            int ran_count ;                         /**< trick_io(*io) trick_units(--) */
            /** @userdesc Last simulation time that this event ran an action.\n */
            double ran_time ;                       /**< trick_io(*io) trick_units(s) */
            /** Array of event's conditions.\n */
            condition_t ** condition_list ;         /**< trick_io(*io) trick_units(--) */
            /** Array of event's actions.\n */
            action_t ** action_list ;               /**< trick_io(*io) trick_units(--) */

            /**
             @brief Constructor.
            */
            IPPythonEvent() ;

            virtual ~IPPythonEvent() ;

            /**
             @brief @userdesc Command to make event evaluation require that ALL of event's conditions be true to make action(s) run.
             @par Python Usage:
             @code <event_object>.condition_all() @endcode
             @return always 0
            */
            int condition_all() ;

            /**
             @brief @userdesc Command to make event evaluation require that ANY of event's conditions be true to make action(s) run (this is the default).
             @par Python Usage:
             @code <event_object>.condition_any() @endcode
             @return always 0
            */
            int condition_any() ;

            /**
             @brief @userdesc Command to manually fire the event next cycle and hold it fired (enter manual mode, bypasses normal condition processing).
             @par Python Usage:
             @code <event_object>.manual_on() @endcode
            */
            virtual void manual_on() ;

            /**
             @brief @userdesc Command to manually fire the event once NOW (enter manual mode, bypasses normal condition processing).
             @par Python Usage:
             @code <event_object>.manual_fire() @endcode
            */
            virtual void manual_fire() ;

            /**
             @brief @userdesc Command to manually set the event as not fired (enter manual mode, bypasses normal condition processing).
             @par Python Usage:
             @code <event_object>.manual_off() @endcode
            */
            virtual void manual_off() ;

            /**
             @brief @userdesc Command to return to normal event processing (needed to end manual mode after any manual commands).
             @par Python Usage:
             @code <event_object>.manual_done() @endcode
            */
            virtual void manual_done() ;

            /**
             @brief @userdesc Command to turn on event info messages (e.g., get a message each time an event fires).
             Event info messages will be published using the Trick status message system.
             @par Python Usage:
             @code trick.set_event_info_msg_on() @endcode
             @return always 0
            */
            static void set_event_info_msg_on() ;

            /**
             @brief @userdesc Command to turn off event info messages (off is the default).
             No event info messages will be published using the Trick status message system.
             @par Python Usage:
             @code trick.set_event_info_msg_off() @endcode
             @return always 0
            */
            static void set_event_info_msg_off() ;

            /**
             @brief called by the event manager when the event is loaded from a checkpoint
            */
            virtual void restart() ;

            /**
             @brief @userdesc Command to create a new condition and set its input string (or reset an existing condition string), num is index starting at 0.
             @par Python Usage:
             @code <event_object>.condition(<num>, """<str>""") @endcode
             @param num - number identifying the condition, starting at 0 for the 1st condition, 1 for the 2nd, etc.
             @param str - the condition input boolean expression, using Python syntax, to be evaluated each cycle
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @param ref     - optional reference to a model variable to be used for the condition (this parameter for internal use only!)
             @param job     - optional pointer to job to be called as the condition (this parameter for internal use only!)
             @return always 0
            */
            int condition(int num, std::string str, std::string comment="", REF2* ref=NULL, JobData* job=NULL) ;

            /**
             @brief @userdesc Command to create a new condition using a boolean model variable (or reset an existing condition variable), num is index starting at 0.
             Using a model variable is more optimal than a normal condition() because there is no Python string to parse.
             @par Python Usage:
             @code <event_object>.condition_var(<num>, """<varname>""") @endcode
             @param num - number identifying the condition, starting at 0 for the 1st condition, 1 for the 2nd, etc.
             @param varname - the name of the boolean model variable, to be evaluated each cycle
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @return always 0
            */
            int condition_var(int num, std::string varname, std::string comment="") ;

            /**
             @brief @userdesc Command to create a new condition using a model job return val (or reset an existing condition job), num is index starting at 0.
             Using a model job return value is more optimal than a normal condition() because there is no Python string to parse.
             @par Python Usage:
             @code <event_object>.condition_job(<num>, """<jobname>""") @endcode
             @param num - number identifying the condition, starting at 0 for the 1st condition, 1 for the 2nd, etc.
             @param jobname - the name of the model job, to be called and evaluated each cycle
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @return always 0
            */
            int condition_job(int num, std::string jobname, std::string comment="") ;

            /**
             @brief @userdesc Command to set an existing condition to hold, so that when it fires it stays in the fired state.
             @par Python Usage:
             @code <event_object>.condition_hold_on(<num>) @endcode
             @param num - number identifying the condition
             @return always 0
            */
            int condition_hold_on(int num) ;

            /**
             @brief @userdesc Command to set an existing condition to not hold its fired state (default).
             @par Python Usage:
             @code <event_object>.condition_hold_off(<num>) @endcode
             @param num - number identifying the condition
             @return always 0
            */
            int condition_hold_off(int num) ;

            /**
             @brief @userdesc Command to enable an existing condition (default is enabled) so that it is evaluated during event processing.
             @par Python Usage:
             @code <event_object>.condition_enable(<num>) @endcode
             @param num - number identifying the condition
             @return always 0
            */
            int condition_enable(int num) ;

            /**
             @brief @userdesc Command to disable an existing condition so that it is not evaluated during event processing.
             @par Python Usage:
             @code <event_object>.condition_disable(<num>) @endcode
             @param num -number identifying the condition
             @return always 0
            */
            int condition_disable(int num) ;

            /**
             @brief @userdesc Accessor function to test if the condition, indicated by num, fired (is currently in the fired state).
             @par Python Usage:
             @code <my_bool> = <event_object>.condition_fired(<num>) @endcode
             @param num - number identifying the condition
             @return true if the specified condition fired in the most recent cycle that the event was evaluated
            */
            bool condition_fired(int num) ;

            /**
             @brief @userdesc Accessor function to see how many times the condition, indicated by num, fired.
             @par Python Usage:
             @code <my_int> = <event_object>.condition_fired_count<(num>) @endcode
             @param num - number identifying the condition
             @return number of times the specified condition has fired
            */
            int condition_fired_count(int num) ;

            /**
             @brief @userdesc Accessor function to see when was the last time the condition, indicated by num, fired.
             @par Python Usage:
             @code <my_double> = <event_object>.condition_fired_time(<num>) @endcode
             @param num - number identifying the condition
             @return the last simulation time that this condition fired (-1.0 if never fired)
            */
            double condition_fired_time(int num) ;

            /**
             @brief @userdesc Accessor function to return the condition string, indicated by num.
             @par Python Usage:
             @code <my_string> = <event_object>.condition(<num>) @endcode
             @param num - number identifying the condition
             @return the specified condition's input boolean expression
            */
            std::string condition_string(int num) ;


            /**
             @brief @userdesc Command to create a new action and set its input string (or reset an existing action string), num is index starting at 0.
             @par Python Usage:
             @code <event_object>.action(<num>, """<str>""") @endcode
             @param num - number identifying the action, starting at 0 for the 1st action, 1 for the 2nd, etc.
             @param str - the action input statement(s), using Python syntax, to be run (processed) when this event is fired
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @param job     - optional pointer to job to be called in the action (this parameter for internal use only!)
             @param act_type - optional type of action string: 0=python 1=Job ON, 2=Job OFF, 3=Job Call (this parameter for internal use only!)
             @return always 0
            */
            int action(int num, std::string str, std::string comment="", JobData* job=NULL, int act_type=0) ;

            /**
             @brief @userdesc Command to create a new action using a model job (or reset an existing action job), num is index starting at 0.
             Using a model job is more optimal than a normal action() because there is no Python string to parse.
             @par Python Usage:
             @code <event_object>.action_job(<num>, """<jobname>""") @endcode
             @param num - number identifying the action, starting at 0 for the 1st action, 1 for the 2nd, etc.
             @param jobname - the action job name of the job to be called when this event is fired
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @return always 0
            */
            int action_job(int num, std::string jobname, std::string comment="") ;

            /**
             @brief @userdesc Command to create a new action to turn a model job ON, num is index starting at 0.
             @par Python Usage:
             @code <event_object>.action_job_on(<num>, """<jobname>""") @endcode
             @param num - number identifying the action, starting at 0 for the 1st action, 1 for the 2nd, etc.
             @param jobname - the action job name of the job to enable when this event is fired
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @return always 0
            */
            int action_job_on(int num, std::string jobname, std::string comment="") ;

            /**
             @brief @userdesc Command to create a new action to turn a model job OFF, num is index starting at 0.
             @par Python Usage:
             @code <event_object>.action_job_off(<num>, """<jobname>""") @endcode
             @param num - number identifying the action, starting at 0 for the 1st action, 1 for the 2nd, etc.
             @param jobname - the action job name of the job to disable when this event is fired
             @param comment - optional description to be displayed in mtv (defaults to 1st 50 characters of str)
             @return always 0
            */
            int action_job_off(int num, std::string jobname, std::string comment="") ;

            /**
             @brief @userdesc Command to enable an existing action (default is enabled) so that it is run when fired during event processing.
             @par Python Usage:
             @code <event_object>.action_enable(<num>) @endcode
             @param num - number identifying the action
             @return always 0
            */
            int action_enable(int num) ;

            /**
             @brief @userdesc Command to disable an existing action so that it is not run when fired during event processing.
             @par Python Usage:
             @code <event_object>.action_disable(<num>) @endcode
             @param num - number identifying the action
             @return always 0
            */
            int action_disable(int num) ;

            /**
             @brief @userdesc Accessor function to test if the action, indicated by num, ran.
             @par Python Usage:
             @code <my_bool> = <event_object>.action_ran(<num>) @endcode
             @param num - number identifying the action
             @return true if the specified action ran at least once
            */
            bool action_ran(int num) ;

            /**
             @brief @userdesc Accessor function to see how many times the action, indicated by num, ran.
             @par Python Usage:
             @code <my_int> = <event_object>.action_ran_count(<num>) @endcode
             @param num - number identifying the action
             @return number of times the specified action ran
            */
            int action_ran_count(int num) ;

            /**
             @brief @userdesc Accessor function to see when was the last time the action, indicated by num, ran.
             @par Python Usage:
             @code <my_double> = <event_object>.action_ran_time(<num>) @endcode
             @param num - number identifying the action
             @return the last simulation time that this action ran (-1.0 if never ran)
            */
            double action_ran_time(int num) ;

            virtual int process( long long curr_time ) ;

            bool process_user_event( long long curr_time ) ;

            virtual void add() ;
            virtual void remove() ;

            /* A static method that allows us to set the IPPython * from the S_define sim_object level without
               any events instantiated yet */
            static void set_python_processor(Trick::IPPython * in_ip) ;
            static void set_mtv(Trick::MTV * in_mtv) ;

        private:

            /* A static pointer to the python input processor set at the S_define level */
            static Trick::IPPython * ip ;

            /* A static pointer to the MTV set at the S_define level */
            static Trick::MTV * mtv ;

    } ;

}

Trick::IPPythonEvent * ippython_new_event(std::string event_name = "" ) ;
int ippython_delete_event(Trick::IPPythonEvent * in_event) ;
int ippython_add_event(Trick::IPPythonEvent * in_event) ;
int ippython_add_event_before( Trick::IPPythonEvent * in_event, std::string target_name, unsigned int target_inst = 1) ;
int ippython_add_event_after( Trick::IPPythonEvent * in_event, std::string target_name, unsigned int target_inst = 1) ;
int ippython_add_read( unsigned int thread_id , double in_time , char * in_string ) ;
int ippython_add_read( double in_time , char * in_string ) ;
int ippython_manual_on(std::string event_name) ;
int ippython_manual_fire(std::string event_name) ;
int ippython_manual_off(std::string event_name) ;
int ippython_manual_done(std::string event_name) ;

#ifdef SWIG
// SWIG macros are not normally run on classes in sim_services.  We can add the the processed code manually
%trick_cast_as(Trick::IPPythonEvent, Trick__IPPythonEvent)
#endif

#endif

