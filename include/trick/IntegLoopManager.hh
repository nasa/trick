/*
PURPOSE:
    ( Integrator "Sim object" )
PROGRAMMERS:
    (((David Hammen) (Odyssey) (2011)))
    (((Donna Panter) (L3)      (2012)))
    (((David Hammen) (Odyssey) (2013)))
*/

#ifndef INTEGLOOPMANAGER_HH
#define INTEGLOOPMANAGER_HH

// Trick includes
#include "trick/SimObject.hh"
#include "trick/Scheduler.hh"

// System includes
#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace Trick {

class IntegLoopScheduler;

    /**
     * The integration manager keeps track of the jobs pertaining to
     * integration that are associated with the various sim objects that
     * comprise a simulation.
     *
     * This class is intended to be used by the IntegLoopScheduler only.
     */
    class IntegrationManager {
    public:

        // Member functions

        /**
         * Default constructor.
         */
        IntegrationManager ();


        /**
         * Is the manager uninitialized?
         * @return True if the sim_objects_info is empty, false otherwise.
         */
        bool is_empty ()
        {
            return sim_objects_info.empty();
        }

        /**
         * Construct information on sim objects related to integration.
         * Each sim object in the simulation is represented.
         */
        void build_sim_object_info ();

        /**
         * Clear the sim objects information; used in restart.
         */
        void clear_sim_object_info ();

        /**
         * Clear the integrated_by designator of objects that are integrated by
         * the specified scheduler.
         * @param scheduler  Integration loop scheduler.
         */
        void clear_integrated_by (Trick::IntegLoopScheduler * scheduler);

        /**
         * Get the scheduler that integrates the specified sim object.
         * @param sim_object  Simulation object.
         * @return Pointer to scheduler (possibly null) that integrates the
         *         specified sim object.
         */
        Trick::IntegLoopScheduler * get_integrated_by (
            Trick::SimObject * sim_object);

        /**
         * Designate the specified sim object as integrated by the specified
         * scheduler.
         * @param sim_object  Simulation object.
         * @param integrator  Integration loop scheduler.
         */
        void set_integrated_by (
            Trick::SimObject * sim_object,
            Trick::IntegLoopScheduler * integrator);

        /**
         * Add jobs of the specified class to the specified queue.
         * @param job_class_id  Identifier for an integration-type job class.
         * @param sim_object    Simulation object.
         * @param queue         Queue to which jobs are to be added.
         */
        void add_jobs_to_queue (
            const std::string & job_class_name,
            int job_class_id,
            Trick::SimObject * sim_object,
            Trick::ScheduledJobQueue & queue);

        /**
         * Find the sim object that corresponds to the sim object derived class
         * pointer (cast to a void pointer).
         * @param derived_sim_object  A sim object, dynamic_cast to void*.
         * @return The SimObject pointer that corresponds to the input pointer.
         */
        Trick::SimObject * find_sim_object (const void * derived_sim_object);


    private:

        // Types

        /**
         * Vector of JobData pointers.
         */
        typedef std::vector<Trick::JobData*> JobVector;


       /**
        * Contains the jobs of some job class of some sim object,
        * with the jobs split into two groups: jobs 'owned' by the sim object,
        * and jobs 'relegated' to some other sim object.
        *
        * The distinction between 'owned' versus 'relegated' is made outside
        * this class.
        */
        class JobClassJobs {

        // Everything is public 'cause this is a dumb internal class.
        public:

            // Member functions

            /**
             * Default constructor.
             */
            JobClassJobs () : owned_jobs(), relegated_jobs() {}

            /**
             * Copy constructor.
             */
            JobClassJobs(const JobClassJobs & src)
            :
                owned_jobs(src.owned_jobs),
                relegated_jobs(src.relegated_jobs)
            {}

            /**
             * Copy-and-swap assignment operator.
             */
            JobClassJobs& operator= (JobClassJobs src)
            {
                swap (*this, src);
                return *this;
            }


            /**
             * Swap as a friend function.
             */
            friend void swap (JobClassJobs & a, JobClassJobs & b)
            {
                std::swap (a.owned_jobs, b.owned_jobs);
                std::swap (a.relegated_jobs, b.relegated_jobs);
            }


            // Member data

            /*
             * The jobs 'owned' by the sim object, either directly or
             * as a relegated job assigned to the sim object.
             */
            JobVector owned_jobs; // trick_io(**)

            /*
             * The jobs registered with the sim object that are
             * relegated to some other sim object.
             */
            JobVector relegated_jobs; // trick_io(**)

        };  // end of class JobClassJobs


        /**
         * Contains information regarding a sim object needed by the
         * integration manager.
         */
        class SimObjectIntegInfo {
        public:

            // Types
            typedef std::map<std::string, JobClassJobs*> JobClassMap;

            /**
             * Default constructor.
             */
            SimObjectIntegInfo ()
            :
                sim_object(NULL),
                integrated_by(NULL)
            {
                init_job_class_to_info();
            }

            /**
             * Non-default constructor.
             * @param sim_object_in  The sim object described by this object.
             */
            explicit SimObjectIntegInfo (Trick::SimObject * const sim_object_in)
            :
                sim_object(sim_object_in),
                integrated_by(NULL)
            {
                init_job_class_to_info();
            }

            /**
             * Copy constructor.
             */
            SimObjectIntegInfo (const SimObjectIntegInfo & src)
            :
                sim_object(src.sim_object),
                integrated_by(src.integrated_by),
                deriv_jobs(src.deriv_jobs),
                pre_integ_jobs(src.pre_integ_jobs),
                integ_jobs(src.integ_jobs),
                dynamic_event_jobs(src.dynamic_event_jobs),
                post_integ_jobs(src.post_integ_jobs)
            {
                init_job_class_to_info();
            }

            /**
             * Copy-and-swap assignment operator.
             */
            SimObjectIntegInfo& operator= (SimObjectIntegInfo src)
            {
                swap (*this, src);
                return *this;
            }

            /**
             * Swap as a friend function.
             */
            friend void swap (SimObjectIntegInfo & a, SimObjectIntegInfo & b)
            {
                using std::swap;

                swap (a.sim_object, b.sim_object);
                swap (a.integrated_by, b.integrated_by);

                swap (a.deriv_jobs, b.deriv_jobs);
                swap (a.pre_integ_jobs, b.pre_integ_jobs);
                swap (a.integ_jobs, b.integ_jobs);
                swap (a.dynamic_event_jobs, b.dynamic_event_jobs);
                swap (a.post_integ_jobs, b.post_integ_jobs);

                swap (a.job_class_to_info, b.job_class_to_info);
            }


            // Member data

            /**
             * The sim object that this SimObjectInfo represents.
             */
            Trick::SimObject * sim_object; // trick_units(--)

            /**
             * The integration loop responsible for integrating the sim object.
             */
            Trick::IntegLoopScheduler * integrated_by; // trick_units(--)

            /**
             * Derivative jobs contained in / assigned to the sim object.
             */
            JobClassJobs deriv_jobs; // trick_units(--)

            /**
             * Pre-integration jobs contained in / assigned to the sim object.
             */
            JobClassJobs pre_integ_jobs; // trick_units(--)

            /**
             * Integration jobs contained in / assigned to the sim object.
             */
            JobClassJobs integ_jobs; // trick_units(--)

            /**
             * Dynamic event jobs contained in / assigned to the sim object.
             */
            JobClassJobs dynamic_event_jobs; // trick_units(--)

            /**
             * Post-integration jobs contained in / assigned to the sim object.
             */
            JobClassJobs post_integ_jobs; // trick_units(--)

            /**
             * Maps job class names to JobClassJobs instances.
             */
            JobClassMap job_class_to_info; // trick_units(--)


        private:

            // Member functions

            /**
             * Complete construction, called by all constructors.
             */
            void init_job_class_to_info ();
        };  // end of class SimObjectIntegInfo

    private:

        // Types

        /**
         * Vector of SimObjectIntegInfo objects.
         */
        typedef std::vector<SimObjectIntegInfo> SimObjectIntegInfoVector;

        /**
         * Defines a unary predicate for searching the sim_objects_info array
         * for the specified SimObject pointer.
         */
        struct FindSimObject {
            FindSimObject (const Trick::SimObject* s) : to_find(s) {}

            bool operator() (const SimObjectIntegInfo &info)
            {
                return info.sim_object == to_find;
            }

            const Trick::SimObject* to_find;
        };

        /**
         * Defines a unary predicate for searching the sim_objects_info array
         * for a specified SimObject pointer, cast to void*.
         * @note: Dynamic cast to void* returns a pointer to the "most derived
         * object." See C++ standard section 5.2.7 paragraph 7.
         */
        struct FindDerivedSimObject {
            FindDerivedSimObject (const void* s) : to_find(s) {}

            bool operator() (const SimObjectIntegInfo &info)
            {
                return dynamic_cast<const void*>(info.sim_object) == to_find;
            }

            const void* to_find;
        };


        // Member functions

        /**
         * Create a SimObjectIntegInfo that represents the specified sim object,
         * adding the representation to the set of such and populating the
         * object with the jobs in the sim object that pertain to integration.
         * @param sim_object The sim object to be represented.
         */
        void create_sim_object_info (Trick::SimObject * sim_object);

        /**
         * Transfer the relegated jobs in a SimObjectIntegInfo to the
         * owned jobs in the SimObjectIntegInfo specified by the relegated job's
         * sup_class_data handle.
         * @param sim_obj_info  The SimObjectIntegInfo to be processed.
         */
        void transfer_relegated_jobs (SimObjectIntegInfo & sim_obj_info);

        /**
         * Find the representation (if any) of the specified sim object.
         * @return Pointer to the SimObjectIntegInfo that represents the
         *         input sim object.
         * @param sim_object    Pointer to the sim object to be found.
         * @param allow_rebuild The sim_objects_info array is rebuilt and
         *                      re-searched if the object is not found and
         *                      and if this flag is set.
         */
        SimObjectIntegInfo* find_sim_object_info (
            const Trick::SimObject * sim_object, bool allow_rebuild=true);

        /**
         * Find the representation (if any) of the specified sim object.
         * @return Pointer to the SimObjectIntegInfo that represents the
         *         input sim object.
         * @param derived_sim_object   Pointer to the sim object to be found.
         * @param allow_rebuild       The sim_objects_info array is rebuilt and
         *                            re-searched if the object is not found and
         *                            and if this flag is set.
         */
        SimObjectIntegInfo* find_sim_object_info (
            const void * derived_sim_object, bool allow_rebuild=true);


        // Member data

        /**
         * Vector of SimObjectIntegInfo instances, one for each SimObject
         * in the simulation.
         */
        SimObjectIntegInfoVector sim_objects_info; //!< trick_io(**)


    }; // end of class IntegrationManager

}; // end of namespace Trick

#endif
