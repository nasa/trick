
/*
PURPOSE:
    (Trick runtime simulation executive scheduler.)
LIBRARY_DEPENDENCIES:
    (SegmentedExecutive/src/Segment.cpp)
*/

#ifndef SEGMENT_HH
#define SEGMENT_HH

#include <string>
#include <map>
#include "sim_services/ScheduledJobQueue/include/ScheduledJobQueue.hh"

namespace Trick {

class Segment {
   public:
        /* This 1 argument constructor creates a new segment, but does not associate any job classes
           with the enter and exit job queues. */
        Segment( std::string segment_name ) ;

        /* This 3 argument constructor associates the enter_job_class_str with the enter_jobs queue and
           exit_job_class_str with the exit_jobs queue.  This should be the normal constructor to use. */
        Segment( std::string segment_name , std::string enter_job_class_str , std::string exit_job_class_str ) ;

        virtual ~Segment() {} ;

        /* getters and setters */
        std::string get_segment_name() ;
        void set_segment_name(std::string in_segment_name) ;
        void set_enter_job_class_name(std::string job_class_name) ;
        void set_exit_job_class_name(std::string job_class_name) ;

        /* Called by the executive as we enter this segment. The previous segment string is
           sent to this routine.  An empty string is sent as the prev_segment if no segment
           was set previously.  */
        virtual int enter( std::string prev_segment ) ;

        /* Called by the executive as we exit this segment.  The next segment string is
           sent to this routine */
        virtual int exit( std::string next_segment ) ;

        /* Used by the executive to pull in all job_classes and their queues */
        std::map < std::string , Trick::ScheduledJobQueue * > & get_job_queues() ;

    protected:
        /* Name of this segment to be used in S_define job tags */
        std::string segment_name ;

        /* Job class string for entering this segment  */
        std::string enter_job_class_name ;

        /* Job class string for exiting this segment  */
        std::string exit_job_class_name ;

        /* Maps job class names to job queues.  All job classes a user wants the executive
           to recognize must reside in this map.  This is the only thing the executive
           reads to get a segment's job classes. */
        std::map < std::string , Trick::ScheduledJobQueue * > job_queues ;

        /* holds jobs to execute when we enter segment */
        Trick::ScheduledJobQueue enter_jobs ;   /**< trick_io(**) */

        /* holds jobs to execute when we exit segment */
        Trick::ScheduledJobQueue exit_jobs ;   /**< trick_io(**) */
} ;

}

#endif

