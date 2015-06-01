/*
PURPOSE:
    ( The job class that the executive schedules and runs )

PROGRAMMERS:
    ((Alex Lin) (NASA) (May 2009) (OO))
*/

#ifndef SCHEDULEDQUEUEINSTRUMENT_HH
#define SCHEDULEDQUEUEINSTRUMENT_HH

#include <string>

#include "trick/InstrumentBase.hh"
#include "trick/JobData.hh"

namespace Trick {

    /**
     * This class is the ScheduledJobQueueInstrument class.  This class contains extra job data that
     * points to the instrumentation's target job as well as event data used by some types of
     * instrumentation jobs.
     *
     * @author Dan A. Strauss
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */
    class ScheduledJobQueueInstrument : public Trick::InstrumentBase {

        public:

            /** The instrumentation job itself\n */
            Trick::JobData * instru_job ;          /**<  trick_io(**) */

            /** The target job of the instrumentation (instrument is inserted before or after target).\n */
            Trick::JobData * target_job ;          /**<  trick_io(**) */

            /**
             * Constructor\n
             * @param inst_job - instrumentation base job as listed in S_define file
             * @param targ_job - target job that is to be instrumented
             */
            ScheduledJobQueueInstrument( Trick::JobData* inst_job, Trick::JobData* targ_job);

            ~ScheduledJobQueueInstrument();

            /**
             * Calls the instumentation jobs only
             * @return always 0
             */
            virtual int call() ;
    } ;

} ;

#endif

