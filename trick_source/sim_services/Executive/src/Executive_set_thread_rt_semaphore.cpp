
/* 
 * $Id:$
 */

#include "sim_services/Executive/include/Executive.hh"

int Trick::Executive::set_thread_rt_semaphores(unsigned int thread_id , bool yes_no) {

    int ret = 0 ;

    /** @par Detailed Design */
    if ( (thread_id +1) > threads.size() ) {
        /** @li If the thread_id does not exist, return an error */
        ret = -2 ;
    } else {
        threads[thread_id]->rt_semaphores = yes_no ;
    }

    return(ret) ;

}

