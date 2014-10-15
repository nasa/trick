/*******************************************************************************
PURPOSE:
    (Real-time Frame logging without SimBus jobs or Timeline)
REFERENCES:
    (See Trick FrameLog class)
LANGUAGE:
    (C++)
ASSUMPTIONS AND LIMITATIONS:
    ((None))
LIBRARY DEPENDENCY:
    ((SnapFrameLog.cpp))
PROGRAMMERS:
    (((Keith)) 
     ((Thomas Brain))
*******************************************************************************/

#ifndef _SNAPFRAMELOGOBJECT_HH_
#define _SNAPFRAMELOGOBJECT_HH_

#include "sim_services/FrameLog/include/FrameLog.hh"
#include "STLHelper/Vector.hh"

/**
  This class provides (optional) logging of Trick frame timing and job performance statistics.
  @author Danny Strauss
 */

class SnapFrameLog {

    public:

        class JobDataItem {
           public:
              JobDataItem() {
                 instance_id = 0;
              }
              JobDataItem(std::string nameIn, unsigned int instanceIn) : name(nameIn) {
                 instance_id = instanceIn;
              }
              std::string name;
              unsigned int instance_id;
        };


        /** Enable real-time frame logging.\n */
        bool frame_log_flag ;                  /**< trick_io(*io) trick_units(--) */

        /** Data recording groups for logging user jobs.\n */
        std::vector< Trick::FrameDataRecordGroup *> drg_users ; /**< trick_io(*io) trick_units(--) */
        /** Data recording group for logging trick jobs.\n */
        Trick::FrameDataRecordGroup * drg_trick;    /**<  trick_io(*io) trick_units(--) */
        /** Data recording group for logging frame/overrun time.\n trick_units(--) */
        Trick::FrameDataRecordGroup * drg_frame;    /**<  trick_io(*io) trick_units(--) */

        /** Number of threads in this sim.\n */
        int num_threads;                /**<  trick_io(**) */

        /** Fake attributes to use for setting up data recording.\n */
        ATTRIBUTES time_value_attr ;    /**<  trick_io(**) */

        /** Save the name of the trick messaging sim object.\n */
        std::string msg_sim_object_name;     /**<  trick_io(**) */
        /** Save the name of the trick data recording sim object.\n */
        std::string dr_sim_object_name;      /**<  trick_io(**) */
        /** Save the name of the trick real-time sync sim object.\n */
        std::string rt_sim_object_name;      /**<  trick_io(**) */
        /** Save the name of the trick master/slave sim object.\n */
        std::string ms_sim_object_name;      /**<  trick_io(**) */

        JobDataItem * _userSpecJobList;

        /**
         @brief Constructor.
        */
        SnapFrameLog() ;

        /**
         @brief Destructor.
        */
        ~SnapFrameLog() ;

        void addJob(std::string jobName, unsigned int instance = 1);

        /**
         @brief Instrumentation job to save job timeline start time.
         @param curr_job - pointer to self (current frame_clock_start job)
        */
        int frame_clock_start(Trick::JobData * curr_job) ;

        /**
         @brief Instrumentation job to save job timeline stop time and frame time.
         @param curr_job - pointer to self (current frame_clock_stop job)
        */
        int frame_clock_stop(Trick::JobData * curr_job) ;

        /**
         @brief @userdesc Command to turn on frame logs.
         @par Python Usage:
         @code trick.frame_log_on() @endcode
         @return always 0
        */
        int framelog_on() ;

        /**
         @brief @userdesc Command to turn off frame logs.
         @par Python Usage:
         @code trick.frame_log_off() @endcode
         @return always 0
        */
        int framelog_off() ;

        /**
         @brief @userdesc Allocates and partially sets up frame logging.
        */
        void default_data() ;

        /**
         @brief Clears data_record informtion realted to frame logging durning the checkpoint reload.
         @return always 0
        */
        int clear_data_record_info() ;

        /**
         @brief Restores the frame log state upon checkpoint restart
         @return always 0
        */
        int restart() ;

        /**
         @brief Shutdown job that writes the job timeline data to disk and closes log files.
         @return always 0
        */
        int shutdown() ;

    protected:
        void addJob( JobDataItem & item );

#if !defined(SWIG) && !defined(TRICK_ICG)
        STLHelper::vector< JobDataItem > userSpecJobList;
        std::vector< Trick::JobData *> jobVector;
#endif


    private:
        std::vector<std::string> trick_jobs;
        std::vector<std::string> user_jobs;

        void allocate_recording_groups() ;

        void add_recording_vars_for_jobs() ;
        void add_recording_vars_for_frame() ;

        void add_recording_groups_to_sim() ;
        void remove_recording_groups_from_sim() ;

        void enable_recording_groups() ;
        void disable_recording_groups() ;
        void init_recording_groups() ;

        void add_instrument_jobs() ;
        void remove_instrument_jobs() ;

} ;

#endif
