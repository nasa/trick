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

#ifndef EVENTMANAGER_HH
#define EVENTMANAGER_HH

#include <string>
#include <vector>

#include "trick/Event.hh"
#include "trick/EventInstrument.hh"
#include "trick/EventProcessor.hh"

namespace Trick {

/**
  This class is handles scheduling for events in the simulation.  It handles adding and
  removing events to the simulation.  It is responsible for maintaining an event processor
  for each thread of the simulation.
  @author Alex Lin, Danny Strauss
*/

    class EventManager {

        public:

            /**
             @brief Constructor.
            */
            EventManager() ;

            /**
             @brief Command to get the event object given the event's name (needed after a reload from checkpoint).
             @param event_name - the string name of the event object you want
             @return the event object if found, NULL if not found
            */
            Trick::Event * get_event(std::string event_name) ;


            /**
             @brief @userdesc Command to insert a user's input file event into the input processor's list of events to process.
             The event will be processed at the top of each execution frame.
             @par Python Usage:
             @code trick.add_event(<in_event>) @endcode
             @param in_event - the event object previously created by new_event()
             @return always 0
            */
            int add_event(Trick::Event * in_event) ;

            /**
             @brief @userdesc Command to insert an instrument job (containing in_event) into job queue before target job.
             The event will be processed immediately before the target job.
             @par Python Usage:
             @code trick.add_event_before(<in_event>, "<target_job>") @endcode
             @param in_event - the event object previously created by new_event()
             @param target_job - name or ID of the job (taken from S_job_execution file) to insert event before
             (ID is the unique number identifying the job, handy when there are multiple jobs with same name)
             @return always 0
            */
            int add_event_before(Trick::Event * in_event, std::string target_job, unsigned int target_inst = 1 ) ;

            /**
             @brief @userdesc Command to insert an instrument job (containing in_event) into job queue after target job.
             The event will be processed immediately after the target job.
             @par Python Usage:
             @code trick.add_event_after(<in_event>, "<target_job>") @endcode
             @param in_event - the event object previously created by new_event()
             @param target_job - name or ID of the job (taken from S_job_execution file) to insert event after
             (ID is the unique number identifying the job, handy when there are multiple jobs with same name)
             @return always 0
            */
            int add_event_after(Trick::Event * in_event, std::string target_job, unsigned int target_inst = 1 ) ;

            /**
             @brief @userdesc Activates an event with a specific name. If you have the <event_object> created by new_event(),
             it's more conventional to call <event_object>.activate().
             @par Python Usage:
             @code trick.activate_event(<event_name>) @endcode
             @param event_name - string containing the event name.
             @return always 0
            */
            int activate_event(const char * event_name) ;

            /**
             @brief @userdesc Deactivates an event with a specific name. If you have the <event_object> created by new_event(),
             it's more conventional to call <event_object>.deactivate().
             @par Python Usage:
             @code trick.deactivate_event(<event_name>) @endcode
             @param event_name - string containing the event name.
             @return always 0
            */
            int deactivate_event(const char * event_name) ;

            /**
             @brief @userdesc Command to remove an event from everywhere it was added.
             It will no longer be processed but can still be displayed in MTV and added again later.
             @par Python Usage:
             @code trick.remove_event(<in_event>) @endcode
             @param in_event - the event object previously created by new_event()
             @return always 0
            */
            int remove_event(Trick::Event * in_event) ;

            /**
             @brief Modifies the event firing times according to the time tic change
             @return always 0
            */
            int time_tic_changed() ;

            /**
             @brief Removes instrumented events before a checkpoint is reloaded.
             @return always 0
            */
            int preload_checkpoint() ;

            /**
             @brief Restore event information from checkpointable structures
             @return always 0
            */
            int restart() ;

            /**
             @brief Add an EventProcessor.  One EventProcessor is assigned to each thread in the sim.
             The event processor holds the list of events to be processed per thread.
             @return always 0
            */
            void add_event_processor( Trick::EventProcessor * in_ipep ) ;

        protected:

            /** All active events in the simulation\n */
            Trick::Event ** active_events ;               /**< trick_io(*io) trick_units(--) */

            /** Number of active events\n */
            unsigned int num_active_events ;              /**< trick_io(*io) trick_units(--) */

            /** Number of active events allocated\n */
            unsigned int num_allocated ;                  /**< trick_io(*io) trick_units(--) */

            /** All of the event processors, one per thread. */
            std::vector< Trick::EventProcessor * > event_processors ;  /**< trick_io(**) */

            /** All of the events that have been attached to jobs */
            std::vector< Trick::EventInstrument * > events_instrumented ;  /**< trick_io(**) */

            /**
             @brief Add user's event to manager's list of events; don't add it if it's already in list.
            */
            int add_to_active_events(Trick::Event * in_event) ;
    } ;

}

#endif

