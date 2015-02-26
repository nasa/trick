/* 
 * $Id:$
 */

#include <iostream>

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/CheckPointRestart/include/checkpoint_stl.hh"

/**
@details
-# Save the number of jobs in the scheduler
-# Copy all of the job information in a checkpointable array
-# Save the number of sim_objects in the scheduler
-# Copy all of the sim_objects in a checkpointable array
*/
int Trick::Executive::checkpoint() {

    unsigned int ii ;

    /* save the number of jobs in the scheduler */
    num_all_jobs = all_jobs_vector.size() ;

    /* copy all of the job information in a checkpointable array */
    if ( num_all_jobs > 0 ) {
        all_jobs_for_checkpoint = (Trick::JobData *)TMM_declare_var( TRICK_STRUCTURED, "Trick::JobData", 
         0 , "all_jobs" , 1 , (int *)&num_all_jobs) ;
        for ( ii = 0 ; ii < num_all_jobs ; ii++ ) {
            all_jobs_for_checkpoint[ii] = *(all_jobs_vector[ii]) ;
        }
    }

    checkpoint_stl(sim_objects , std::string("trick_sys") , std::string("sim_objects")) ;
    checkpoint_stl(freeze_times , std::string("trick_sys") , std::string("freeze_times")) ;

    return(0) ;

}
