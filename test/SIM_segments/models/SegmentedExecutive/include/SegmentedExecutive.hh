/*
PURPOSE:
    (Trick runtime simulation executive scheduler.)
LIBRARY_DEPENDENCIES:
    (SegmentedExecutive/src/SegmentedExecutive.cpp)
*/

#ifndef SEGMENTEDEXECUTIVE_HH
#define SEGMENTEDEXECUTIVE_HH

#include <set>
#include <map>
#include <string>
#include "trick/Executive.hh"
#include "SegmentedExecutive/include/Segment.hh"

/**
This class is an example to how to add simulation phasing capabilities. This class adds
several new segment transition job classes.  Users may switch segment at the top of the
frame, or switch segments immediately.
*/

namespace Trick {

class SegmentedExecutive : public Trick::Executive {

    friend class InputProcessor ;
    friend void  init_attrTrick__SegmentedExecutive() ;

    public:
        SegmentedExecutive() ;

        /* Add a new segment */
        int add_segment( Trick::Segment * ps ) ;

        /* S_define level initialization job to gather all jobs that are segment controlled */
        int gather_segmented_jobs() ;

        /* set the next segment. Segment switch does not happen until top of frame */
        int segment_set(std::string in_segment) ;

        /* top_of_frame S_define level job, or may be manually called */
        int segment_switch() ;

    protected:

        /* maps string segment names to their respective objects */
        std::map < std::string , Trick::Segment * > segment_map ;

        /* The current segment */
        Trick::Segment * curr_segment ;

        /* The next segment assigned by segment_set */
        std::string next_segment_str ;

        /** Set of jobs that contain at least one job tag that is a segment */
        std::set< Trick::JobData * > all_segmented_jobs ;    /**< trick_io(**) */

        /* Called for all segment transitions to set jobs on/off */
        int segment_set_jobs_onoff() ;

    public:
        /* These jobs overload ones in Trick::Executive and add the segment specific jobs. */
        virtual int write_s_job_execution( FILE * fp ) ;
        virtual int instrument_job_before(Trick::JobData * instrument_job ) ;
        virtual int instrument_job_after(Trick::JobData * instrument_job) ;
        virtual int instrument_job_remove(std::string in_job) ;

} ;

}

#ifndef TRICK_ICG
extern "C" {
    int exec_segment_set( std::string in_segment ) ;
    int exec_segment_switch() ;
} ;
#endif

#endif

