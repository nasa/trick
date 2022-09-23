#ifndef SIMTIME_HH
#define SIMTIME_HH
/*
    PURPOSE: ( SimTime Class )
*/
#include "trick/time_offset.h"

namespace Trick {

    class SimTime {

        public:
            TIME_OFFSET ret_time ;       /* -- current run elapsed time */

            TIME_OFFSET gmt_ref ;        /* -- gmt ref time */
            double gmt_sec_ref ;         /* -- gmt ref in seconds */
            TIME_OFFSET gmt_time ;       /* -- current gmt time */

            TIME_OFFSET met_ref ;        /* -- met ref time */
            double met_sec_ref ;         /* -- met ref in seconds */
            TIME_OFFSET met_time ;       /* -- current met time */

            SimTime() ;
            int init_times() ;

            double get_rettime() ;
            double get_gmttime() ;
            double get_mettime() ;

            TIME_OFFSET * get_rettime_struct() ;
            TIME_OFFSET * get_gmttime_struct() ;
            TIME_OFFSET * get_mettime_struct() ;
    } ;

}


#endif

