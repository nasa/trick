/*
PURPOSE:
    ( Integrator "Sim object" )
PROGRAMMERS:
    (((Alex Lin)     (NASA)    (2009)))
    (((John Penn)    (L3)      (2010)))
    (((Donna Panter) (L3)      (2012)))
    (((David Hammen) (Odyssey) (2013)))
*/

#ifndef INTEGLOOPSCHEDULER_HH
#define INTEGLOOPSCHEDULER_HH


// Local includes
#include "trick/IntegLoopManager.hh"
#include "trick/Integrator.hh"
#include "trick/IntegAlgorithms.hh"

// Trick includes
#include "trick/SimObject.hh"
#include "trick/Scheduler.hh"

// System includes
#include <vector>
#include <string>
#include <map>
#include <cstdarg>

#ifdef SWIG
// We want SWIG to ignore add_sim_object(Trick::SimObject *)
// so only add_sim_object(Trick::SimObject &) can be called in input file.
%ignore Trick::IntegLoopScheduler::add_sim_object(Trick::SimObject *) ;
#endif

namespace Trick {

    class IntegrationManager;
    class SimObject;

    /**
     * This class is a Scheduler that provides the ability to integrate a
     * collection of sim object instances over time.  The IntegLoopScheduler
     * schedules for following job classes:
     *  - Pre-integration jobs.\n
     *    The pre-integration jobs are the set of jobs that are to be called
     *    prior to advancing state.
     *  - Derivative jobs and integration jobs.\n
     *    State is advanced by repeatedly calling all of the derivative class
     *    jobs and then all of the integration class jobs until the integration
     *    jobs indicate that the target time has been reached.
     *  - Dynamic event jobs.\n
     *    The dynamic event jobs are used to search for events that
     *    intrinsically change the behaviors of the derivatives. Since such
     *    events change behavior, state is be integrated back to the point
     *    where the event occurs and then integrated forward to the desired
     *    target time.
     *  - Post-integration jobs.\n
     *    The post-integration jobs are called after state is advanced to the
     *    target time.
     *  .
     *
     * The set of sim objects to be integrated by an IntegLoopScheduler can be
     * specified at construction time, but can be also changed during runtime.
     * Sim objects can be added to or removed from an IntegLoopScheduler, and
     * can be moved from one IntegLoopScheduler to another.
     */
    class IntegLoopScheduler : public Scheduler {

        public:

            // Types

            /**
             * Vector of sim object pointers.
             */
            typedef std::vector<Trick::SimObject*> SimObjectVector;


            // Member data

            /**
             * Output verbosity, defaults to zero (no debug output).
             */
            int verbosity;  //!< trick_units(--)

            /**
             * Indicates whether derivative jobs need to be called just prior
             * to calling the post-integration class jobs.
             */
            bool last_step_deriv; //!< trick_units(--)

            /**
             * Not used.
             * @warn: Deprecated. Very deprecated.
             */
            bool first_step_deriv; //!< trick_units(--)

            /**
             * Pointer to the Trick::Integrator object that directs
             * the integration process.
             * @warn: Deprecated.
             */
            Trick::Integrator* integ_ptr;  // trick_units(--)

            /**
             * The sim objects integrated by this IntegLoopScheduler.
             * This is public so it can be checkpointed and restored.
             */
            SimObjectVector sim_objects; //!< trick_io(*io) trick_units(--)


            // Member functions

            /**
             * Non-default constructor.
             * @param in_cycle   The time interval at which the loop's integrate
             *                   function will be called.
             * @param parent_so  The Trick simulation object that contains this
             *                   IntegLoopScheduler object.
             */
            IntegLoopScheduler (double in_cycle, Trick::SimObject * parent_so);

            /**
             * Default constructor.
             * @note This exists for checkpoint/restart. Other users should
             * not call this function.
             */
            IntegLoopScheduler ();

            /**
             * Destructor.
             */
            virtual ~IntegLoopScheduler () {}


            /**
             * Add the jobs from the provided sim object that are related to
             * integration jobs to the appropriate integration job queues.
             * @param sim_object  Sim object to be integrated.
             * @return Zero.
             * @warn Deprecated.@n
             * This function exists for backward compatibility
             * and will be deleted in a future release. It is incompatible
             * with the add_sim_object / remove_sim_object schema.
             */
            int add_integ_jobs_from_sim_object (Trick::SimObject * sim_object);


            /**
             * Call the derivative jobs.
             */
            void call_deriv_jobs ();

            /**
             * Integrate state to the current simulation time.
             * Simulation time is advanced prior to calling this function.
             */
            int integrate ();

            /**
               Overload of Scheduler::add_sim_object(Trick::SimObject *) routine.
               This class does not add sim_objects with this routine.  Sim_objects
               are added to the integrator through the add_integ_sim_object call.
             */
            virtual int add_sim_object (Trick::SimObject * sim_obj);

            /**
             * Add the sim object to the set of such to be integrated by this
             * integration loop.
             * A simulation object defines the level granularity for what is /
             * is not integrated by an integration loop object. The integration
             * loop object forms job queues based on the sim objects assigned
             * to the integration loop object via this function.
             * @param sim_obj Object to be integrated by this object.
             * @return Zero = success, non-zero = failure (object not added).
             */
            virtual int add_sim_object (Trick::SimObject &sim_obj);

            /**
             * Remove the sim object from the set of such to be integrated by
             * this integration loop.
             * @param sim_obj Object not to be integrated by this object.
             * @return Zero = success, non-zero = failure (object not removed).
             */
            virtual int remove_sim_object (Trick::SimObject &sim_obj);

            /**
             * Rebuild the integration loop's job queues.
             * This function should be called externally as an S_define level
             * call during initialization and as a restart job.
             *
             * This function is called internally when a sim object is
             * added to or deleted from the set of objects to be integrated
             * during runtime.
             */
            virtual void rebuild_jobs ();

            /**
             * Prepare for a restart from a checkpoint.
             * This function should be called externally as an S_define level
             * preload_checkpoint job.
             */
            virtual void restart_checkpoint ();


            /**
             * Determine if any integrators need derivatives on the first step.
             * @return True if any integrator needs first step derivatives,
             *         false if no integrator needs them.
             */
            bool get_first_step_deriv_from_integrator ();

            /**
             * Deprecated.
             * Very, very deprecated.
             * This function does nothing.
             */
            void set_first_step_deriv (bool first_step __attribute__ ((unused))) {}


            /**
             * Determine derivatives should be computed just before
             * calling the post-integration jobs.
             * @return True if the last_deriv_flag is set or if any integrator
             *         has it's last_step_deriv flag set; false otherwise.
             */
            bool get_last_step_deriv ();

            /**
             * Sets the last_deriv_flag to the specified value.
             * @param last_step  New value for the last_deriv_flag
             */
            void set_last_step_deriv (bool last_step) {
                last_step_deriv = last_step;
            }


            /**
             * Set the verbosity of each integrator associated with this
             * integ_loop scheduler to the specified level.
             * @param level  Verbosity level.
             */
            void set_verbosity (int level) {
                verbosity = level;
            }


            /**
             * Creates an integrator object for use by some integration class
             * job associated with this integration loop.
             * @param alg         Integration technique.
             * @param state_size  State vector size.
             */
            Trick::Integrator * getIntegrator (
                Integrator_type alg, unsigned int state_size);


            /**
             * Get the interval between calls to the integ_loop job.
             * @return Integration cycle time, in Trick seconds.
             */
            double get_integ_cycle () const {
                return next_cycle;
            }

            /**
             * Change the interval between calls to the integ_loop job.
             * @param cycle  New integration cycle time, in Trick seconds.
             */
            virtual int set_integ_cycle (double cycle);


            /**
             * Writes the S_job_execution file, which details the jobs known to
             * the scheduler at the end of initialization.
             * @return always 0
             */
            virtual int write_s_job_execution (FILE * fp);

            /**
             * Adds the specified instrumentation job before each job in each of
             * the job queues related to integration.
             * Requirement [@ref r_exec_instrument_0]
             * @return always 0
             */
            virtual int instrument_job_before (Trick::JobData * instrument_job);

            /**
             * Adds the specified instrumentation job after each job in each of
             * the job queues related to integration.
             * Requirement [@ref r_exec_instrument_2]
             * @return always 0
             */
            virtual int instrument_job_after (Trick::JobData * instrument_job);

            /**
             * Removes an instrumentation job with the name in_job from each of
             * the job queues related to integration.
             * Requirement [@ref r_exec_instrument_3]
             * @return always 0
             */
            virtual int instrument_job_remove (std::string in_job);


        protected:

            // Types

            /**
             * Vector of sim object pointers.
             */
            typedef std::vector<Trick::Integrator*> IntegratorVector;


            // Static member data

            /**
             * The singleton IntegrationManager object that keeps track of all
             * sim objects, including
             *  - The jobs related to integration specified in the sim objects.
             *  - Which, if any, IntegrationLoop integrates the sim object.
             */
            static Trick::IntegrationManager manager;  // trick_units(--)

            // Member data

            /**
             * Interval at which the integration_loop function is to be called
             * nominally, in simulation engine seconds.
             */
            double nominal_cycle; //!< trick_units(s)

            /**
             * Interval at which the integration_loop function is to be called
             * the next time around, in simulation engine seconds.
             */
            double next_cycle; //!< trick_units(s)

            /**
             * The sim object that contains this IntegLoopScheduler.
             */
            SimObject * parent_sim_object; //!< trick_units(--)


            /**
             * Pre-integration jobs managed by this loop.
             */
            Trick::ScheduledJobQueue pre_integ_jobs; //!< trick_units(--)

            /**
             * Derivative jobs managed by this loop.
             */
            Trick::ScheduledJobQueue deriv_jobs; //!< trick_units(--)

            /**
             * Integration jobs managed by this loop.
             */
            Trick::ScheduledJobQueue integ_jobs; //!< trick_units(--)

            /**
             * Dynamic event jobs managed by this loop.
             */
            Trick::ScheduledJobQueue dynamic_event_jobs; //!< trick_units(--)

            /**
             * Post-integration jobs managed by this loop.
             */
            Trick::ScheduledJobQueue post_integ_jobs; //!< trick_units(--)


            // Member functions

            /**
             * Complete the construction of an integration loop.
             * All constructors but the copy constructor call this method.
             */
            void complete_construction (void);

            /**
             * Empty a job queue in anticipation of the queue being rebuilt.
             */
            void clear_queue (Trick::ScheduledJobQueue & job_queue);

            /**
             * Find the specified sim object in the set of integrated objects.
             * @return Iterator pointing to the object,
             *         or to sim_objects.end() if not found.
             * @param sim_obj Object to be found.
             */
            SimObjectVector::iterator find_sim_object (
                Trick::SimObject & sim_obj);


            /**
             * Integrate sim objects over the specified time span.
             * This is an overridable internal integration function that is
             * called by the externally-visible integrate method.
             *
             * @return          Zero/non-zero success indicator.
             *                  Out-of-sync integrators cause a non-zero return.
             * @param beg_time  Time at the start of the integration interval.
             * @param del_time  Time span of the integration interval.
             */
            virtual int integrate_dt (double beg_time, double del_time);


            /**
             * Process dynamic events.
             *
             * @return          Zero/non-zero success indicator.
             *                  Out-of-sync integrators cause a non-zero return.
             * @param start_t  Time at the beginning of the integration interval.
             * @param end_t    Time at the end of the integration interval.
             */
            int process_dynamic_events (double start_t, double end_t, unsigned int depth=0);

    };
}


/**
 * The integrator currently being integrated.
 * This global is used by the C language interface to the integration functions.
 */
extern Trick::Integrator* trick_curr_integ; //!< trick_io(**)

#endif

