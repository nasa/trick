/*
PURPOSE:
    ( Frame data record class)
PROGRAMMERS:
    (((Alex Lin) (NASA) (10/2012)))
*/

/*
 * $Id: FrameLog.hh 2299 2012-04-11 16:13:59Z dstrauss $
 */

#ifndef _FRAMEDATARECORDGROUP_HH_
#define _FRAMEDATARECORDGROUP_HH_

#include <vector>
#include <string>
#include "sim_services/DataRecord/include/DRBinary.hh"
#include "sim_services/include/mm_macros.hh"

namespace Trick {

    class FrameDataRecordGroup : public Trick::DRBinary {

        TRICK_MM_INTERFACE( Trick::FrameDataRecordGroup , Trick__FrameDataRecordGroup )

        public:

            /** Must be instantiated with a thread assignment and a name */
            FrameDataRecordGroup( int thread_id , std::string in_name ) ;

            /** Start the timer at the beginning of the frame */
            void start_timer() ;

            /** Stop the timer at the end of the frame */
            void stop_timer() ;

            /** Clear frame log times for this group */
            int frame_log_clear() ;

            /** Adjusts time for main thread */
            void adjust_time() ;

            /** Overload the enable command from Trick::DataRecordGroup */
            virtual int enable() ;

            /** Add job to list of jobs we are watching. */
            virtual void add_rec_job(Trick::JobData * new_job ) ;

            /** Restart after a checkpoint load. */
            virtual int restart() ;

            /**
             @brief The executive scheduler's interface to data_record job.
             @returns always 0
            */
            virtual int call_function( Trick::JobData * curr_job ) ;

        protected:
            /** thread this group will run on. */
            unsigned int thread_id ;

            /** Jobs this group is recording */
            std::vector< Trick::JobData *> rec_jobs ;

            /** Time value to record with main frame.  The main frame has already incremented time by the time
                data record is called.  We need to record the time of the previous frame. */
            double thread_record_time ;

            /** Thread start time */
            long long start_time ;

            /** Thread total frame scheduled time */
            long long frame_sched_time ;

    } ;

} ;

#endif
