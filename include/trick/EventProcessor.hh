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

#ifndef EVENTPROCESSOR_HH
#define EVENTPROCESSOR_HH

#include <queue>
#include <set>

#include "trick/Event.hh"
#include "trick/JobData.hh"

namespace Trick {

/**
  This class processes events on the thread the class was assigned.  The process_event
  job is an S_define level job that will be scheduled to run on an assigned thread.
  Events are stored in this class and are processed as needed based on the next event's
  execution time.  Events may be added or removed from the event_set.

  @author Alex Lin, Danny Strauss
 */

    class EventProcessor {

        public:

            /**
             @brief Sets the process_event_job pointer.
            */
            void set_process_event_job( Trick::JobData * in_job) { process_event_job = in_job ; } ;

            /**
             @brief Add a new event to the pending events list.  Pending events are added to the
              processing queue at the next top of frame when add_pending_events is run.
            */
            void add_event(Trick::Event * in_event) ;

            /**
             @brief Remove an event from the queue.
            */
            void remove_event(Trick::Event * in_event) ;

            /**
             @brief top_of_frame job that moves pending events to the process_event queue.
             @return always 0
            */
            void add_pending_events(long long curr_time, bool is_restart = false ) ;

            /**
             @brief Automatic job to process input file events.
             @return always 0
            */
            int process_event( long long curr_time ) ;

            /**
             @brief Clears the event set before a checkpoint is loaded
            */
            void preload_checkpoint() ;

        private:

            Trick::JobData * process_event_job ; // trick_io(**)

            /** Use an ordered set to store the events.  The events are sorted by their next execution
                time.  The set allows us to add/remove items at any time.\n */
            std::multiset< Trick::Event *, CompareEventPtrs > event_set ;  // trick_io(**)

            /** Added events are put in the staging area called pending events.  The add_pending_events
                job moves pending events to the event_set.\n */
            std::vector< Trick::Event * > pending_events ; // trick_io(**)

    } ;

}

#endif

