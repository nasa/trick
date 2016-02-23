
/*
PURPOSE:
    (Trick runtime simulation executive scheduler.)
LIBRARY_DEPENDENCIES:
    (SegmentedExecutive/src/ExampleSegments.cpp)
*/

#ifndef EXAMPLESEGMENTS_HH
#define EXAMPLESEGMENTS_HH

#include "SegmentedExecutive/include/Segment.hh"
#include "sim_services/Integrator/include/IntegLoopScheduler.hh"

class SegmentA : public Trick::Segment {
    public:
        SegmentA( std::string segment_name ,
                std::string enter_job_class_str ,
                std::string exit_job_class_str ,
                Trick::IntegLoopScheduler & integ_sched ) ;

        /* overload the enter_state routine so we can call specialized "A" to "B" jobs */
        virtual int enter( std::string next_segment ) ;

    protected:
        Trick::IntegLoopScheduler & integ_sched ;

    private:
        SegmentA &operator=(const SegmentA &);

} ;

class SegmentB : public Trick::Segment {
    public:
        SegmentB( std::string segment_name ,
                std::string enter_job_class_str ,
                std::string exit_job_class_str ,
                Trick::IntegLoopScheduler & integ_sched ) ;

        /* overload the enter_state routine so we can call specialized "A" to "B" jobs */
        virtual int enter( std::string next_segment ) ;

    protected:
        Trick::IntegLoopScheduler & integ_sched ;

    private:
        SegmentB &operator=(const SegmentB &);

} ;

#endif

