/*
PURPOSE:
    (Data Record Destination parameter definition.)
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
*/

#ifndef DATARECORDDISPATCHER_HH
#define DATARECORDDISPATCHER_HH

#include <iostream>
#include <pthread.h>

#include "trick/Scheduler.hh"
#include "trick/DataRecordGroup.hh"
#include "trick/ThreadBase.hh"

namespace Trick {

    class DRDMutexes {
        public:
            DRDMutexes() ;
            /** Data writer thread condition variable.  */
            pthread_cond_t dr_go_cv;        /**< trick_io(**) */
            /** Data writer thread condition mutex.  */
            pthread_mutex_t dr_go_mutex;    /**< trick_io(**) */
            /** Data writer initialized. */
            pthread_cond_t init_complete_cv;        /**< trick_io(**) */
            /** Data writer initialized mutex. */
            pthread_mutex_t init_complete_mutex;    /**< trick_io(**) */
    } ;

    class DRDWriterThread : public Trick::ThreadBase {
        public:
            DRDWriterThread(Trick::DRDMutexes & in_mutexes, std::vector <Trick::DataRecordGroup *> & in_groups) ;

            virtual void * thread_body() ;
            virtual void dump( std::ostream & oss = std::cout ) ;
        protected:
            Trick::DRDMutexes & drd_mutexes ;  // trick_io(**)
            std::vector <Trick::DataRecordGroup *> & groups ;  // trick_io(**)

        private:
            void operator =(const Trick::DRDWriterThread &) ;
    } ;

    /*
       The DataRecordDispatcher inherits from Trick::Scheduler.  It does not contain job classes
       of its own, but we do want to be notified when new sim_objects are added to the simulation.
     */
    class DataRecordDispatcher : public Trick::Scheduler {

        public:

            DataRecordDispatcher() ;
            virtual ~DataRecordDispatcher() ;

            /** @brief Removes old data recording files. */
            int remove_files() ;

            /** @brief Creates a new thread for writing simulation data to disk. */
            int init() ;

            /** @brief Init all groups (only needed if restoring checkpoint during initialization). */
            int init_groups() ;

            /** @brief Adds a new data recording group. */
            int add_group(Trick::DataRecordGroup * in_group, Trick::DR_Buffering buffering = DR_Not_Specified ) ;

            /** @brief Removes a data recording group. */
            int remove_group(Trick::DataRecordGroup * in_group ) ;

            /** @brief Removes all data recording groups. */
            void remove_all_groups() ;

            /** @brief Gets a data recording group. */
            Trick::DataRecordGroup * get_group(std::string group_name) ;

            /** @brief Signal the write thread to execute. */
            virtual int signal_thread() ;

            /** @brief Clears the tracked data record groups before a checkpoint reload. */
            int preload_checkpoint() ;

            /** @brief Calls init() to create thread for writing simulation data to disk during restart. */
            int restart() ;

            /** @brief Calls restart of all data recording groups. */
            int dmtcp_restart() ;

            /** @brief Stop data recording during simulation shutdown. */
            virtual int shutdown() ;

            /** @brief Enable a group or all groups */
            int enable( const char * in_name = NULL ) ;

            /** @brief Disable a group or all groups */
            int disable( const char * in_name = NULL ) ;

            /** @brief Disable a group or all groups */
            int record_now_group( const char * in_name ) ;

            /** @brief set max file size for group */
            int set_group_max_file_size(const char * in_name, uint64_t bytes) ;

            /** @brief set max file size for all groups */
            int set_max_file_size(uint64_t bytes) ;

            // override the default Schduler::add_sim_object
            virtual int add_sim_object( Trick::SimObject * in_object ) ;

            // routines inherited from scheduler... this class does not need to do anything with these.
            virtual int write_s_job_execution( FILE * fp __attribute__((unused))) { return 0 ; } ;
            virtual int instrument_job_before(Trick::JobData * instrument_job __attribute__((unused))) { return 0 ; } ;
            virtual int instrument_job_after(Trick::JobData * instrument_job __attribute__((unused))) { return 0 ; } ;
            virtual int instrument_job_remove(std::string in_job __attribute__((unused))) { return 0 ; } ;

            /** Writer thread */
            DRDWriterThread drd_writer_thread ;

        protected:

            /** All groups using this buffering technique */
            std::vector <Trick::DataRecordGroup *> groups ; /* trick_io(**) trick_units(--) */

            /** mutexes shared with writer thread */
            DRDMutexes drd_mutexes ;  // trick_io(**)


        private:
            void operator =(const Trick::DataRecordDispatcher &) ;

    } ;

} ;

#endif
