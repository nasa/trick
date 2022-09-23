#ifndef EVENT_HH
#define EVENT_HH
/*
    PURPOSE: ( Event Class )
*/
#include <string>
#include "trick/mm_macros.hh"
#include "trick/exec_proto.h"

namespace Trick {

/*
   Is the event a cyclic event, or attached before or after a job
*/
enum EventBeforeAfter {
    EVENT_NOTARGET ,
    EVENT_BEFORETARGET ,
    EVENT_AFTERTARGET
} ;

/**
  This class encapsulates an event into an instrument class.  Instrument classes
  may be attached to jobs.  When this instrument class is called, the event is
  processed.

  @author Alex Lin, Danny Strauss
*/
class Event {

    public:

        /**
         @brief constructor assigns no name to the event with a default cycle time of 1.0 sec.
        */
        Event(std::string in_name = "no_name_specified", double in_cycle = 1.0) :
         name(in_name) ,
         active(false) ,
         thread_id(0) ,
         before_after(EVENT_NOTARGET) ,
         cycle(in_cycle) ,
         next_tics(0) ,
         free_on_removal(false) ,
         target_inst(1) {
            set_cycle(cycle) ;
        }

        virtual ~Event() {} ;
        /**
         @brief get the event name
        */
        std::string get_name() { return name ; } ;

        /**
         @brief set the event name
        */
        void set_name(std::string in_name) { name = in_name ; } ;

        /**
         @brief returns if the event is active or not.
        */
        bool is_active() { return active ; } ;

        /**
         @brief @userdesc Command to turn on processing of event (can be overridden by manual mode).
         @par Python Usage:
         @code <event_object>.activate() @endcode
         @return always 0
        */
        void activate() { active = true ; } ;

        /**
         @brief @userdesc Command to turn off processing of event (can be overriden by manual mode).
         @par Python Usage:
         @code <event_object>.deactivate() @endcode
         @return always 0
        */
        void deactivate() { active = false ; } ;

        /**
         @brief get which thread this event will run on.
        */
        void set_thread(unsigned int in_thread) { thread_id = in_thread ; } ;

        /**
         @brief set which thread this event will run on.
        */
        unsigned int get_thread() { return thread_id ; } ;

        /**
         @brief sets the event to be cyclic or run before or after a job.
        */
        void set_before_after(EventBeforeAfter in_before_after ) { before_after = in_before_after ; } ;

        /**
         @brief gets the event's cyclic status, or is it set to run before/after a job.
        */
        EventBeforeAfter get_before_after() { return before_after ; } ;

        /**
         @brief @userdesc Command to set rate at which event's conditions are evaluated
         @par Python Usage:
         @code <event_object>.set_cycle(<rate>) @endcode
         @return always 0
        */
        void set_cycle(double rate) { cycle = rate ; cycle_tics = (long long)(cycle * exec_get_time_tic_value()) ; } ;

        /**
         @brief Gets the current event cycle in seconds
        */
        double get_cycle() { return cycle ; } ;

        /**
         @brief Gets the current event cycle in tics
        */
        long long get_cycle_tics() const { return cycle_tics ; } ;

        /**
         @brief Gets the next event evaluation time in tics
        */
        long long get_next_tics() const { return next_tics ; } ;

        /** sets next_tics to incoming value */
        void set_next_tics( long long in_tics ) { next_tics = in_tics ; } ;

        /** adds cycle_tics to next_tics */
        void advance_next_tics() { next_tics += cycle_tics ; } ;

        /**
         @brief Gets the status if we free this event once it is removed from the event queue.
        */
        bool get_free_on_removal() { return free_on_removal ; } ;

        /**
         @brief Sets the status if we free this event once it is removed from the event queue.
        */
        void set_free_on_removal() { free_on_removal = true ; } ;

        /**
         @brief For events attached to jobs, sets the target job name
        */
        std::string get_target_name() { return target_name ; } ;

        /**
         @brief For events attached to jobs, gets the target job name
        */
        void set_target_name( std::string in_name ) { target_name = in_name ; } ;

        /**
         @brief For events attached to jobs, gets the target instance number
        */
        unsigned int get_target_inst() { return target_inst ; } ;

        /**
         @brief For events attached to jobs, sets the target instance number
        */
        void set_target_inst(unsigned int in_target_inst) { target_inst = in_target_inst ; } ;

        /** process the event */
        virtual int process( long long curr_time ) = 0 ;

        /** called when the event is added to the event manager */
        virtual void add() = 0 ;

        /** called when the event is removed from the event manager */
        virtual void remove() = 0 ;

        /** called when the event is restarted from a checkpoint */
        virtual void restart() = 0 ;

    protected:

        /** name! */
        std::string name ;

        /** are we active? */
        bool active ;

        /** Thread to run this job. (valid for cyclic/read jobs, not events tied to jobs)\n */
        unsigned int thread_id ;                /**< trick_io(*io) trick_units(--) */

        /** Saves the before/after target of the event.  This parameter is used for checkpointing.\n */
        EventBeforeAfter before_after ;         /**< trick_io(*io) trick_units(--) */

        /** Rate at which event's conditions are evaluated (ignored if event inserted before/after a job).\n */
        double cycle ;                          /**< trick_io(*io) trick_units(s) */

        /** Cycle expressed in tics. */
        long long cycle_tics ;                  /**< trick_io(*io) trick_units(--) */

        /** Next cycle time event will be processed expressed in tics. */
        long long next_tics ;

        /** When this event is removed from event list do we free the memory? */
        bool free_on_removal ;

        /** For events tied to jobs, the target job's name */
        std::string target_name ;

        /** For events tied to jobs, the target job's instance number */
        unsigned int target_inst ;

} ;


#ifndef SWIG
struct CompareEventPtrs : public std::binary_function<Trick::Event *, Trick::Event *, bool> {
    bool operator()(const Trick::Event * lhs, const Trick::Event * rhs) const {
        return lhs->get_next_tics() < rhs->get_next_tics();
    }
};

#endif

}

#endif
