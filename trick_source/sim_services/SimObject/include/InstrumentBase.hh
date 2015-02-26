/*
PURPOSE:
    ( The job class that the executive schedules and runs )

PROGRAMMERS:
    ((Alex Lin) (NASA) (May 2009) (OO))
*/

/*
 * $Id: InstrumentBase.hh 3537 2014-05-27 22:49:43Z alin $
 */

#ifndef _INSTRUMENTBASE_HH_
#define _INSTRUMENTBASE_HH_

#include <string>
#include "sim_services/SimObject/include/JobData.hh"

namespace Trick {

    /**
     * This class is the base Instrument class. It provides an abstract
     * call routine that is used by jobs to call each Instrument's particular routine.
     *
     * @author Dan A. Strauss
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */

    class JobData ;

    class InstrumentBase {

        public:

            InstrumentBase(Trick::JobData * in_target_job = NULL) :
             phase(60000) ,
             target_job(in_target_job) {} ;

            virtual ~InstrumentBase() {} ;

            /* Get the target job. */
            Trick::JobData * get_target_job() { return target_job ; } ;

            /** Job source code name */
            std::string name ;                    /**< trick_units(--) */
            unsigned short phase ;

            virtual int call() = 0 ;

        protected:
             Trick::JobData * target_job ;

    } ;

} ;

#endif

