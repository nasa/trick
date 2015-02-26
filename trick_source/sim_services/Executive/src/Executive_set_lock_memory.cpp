
/* 
 * $Id:$
 */

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Message/include/message_proto.h"

#if __linux 

#include <sys/mman.h>
#include <errno.h>

int Trick::Executive::set_lock_memory(bool yes_no) {

    /** @par Detailed Design */
    int ret = 0 ;

    /** @li lock or unlock memory based on yes_no parameter */
    if ( yes_no ) {
        if ((ret = mlockall(MCL_CURRENT | MCL_FUTURE)) != 0 ) { 
            perror("Error locking memory.");
            message_publish(MSG_ERROR, "Error %d when requesting memory lock.\n", errno);
        } else {
            message_publish(MSG_INFO, "Sim locked memory\n");
        }
    } else {
        if ( (ret = munlockall()) != 0 ) { 
            perror("Error unlocking memory.");
            message_publish(MSG_ERROR, "Error %d when requesting memory unlock.\n", errno);
        } else {
            message_publish(MSG_INFO, "Sim unlocked memory\n");
        }

    }

    /** @li return result of mlockall or munlockall */
    return(ret) ;

}

#endif

#if __APPLE__

int Trick::Executive::set_lock_memory(bool yes_no __attribute__((unused)) ) {

   message_publish(MSG_WARNING, "Warning: Trick on Darwin does not yet support memory locking.\n");

   return(0) ;

}

#endif

