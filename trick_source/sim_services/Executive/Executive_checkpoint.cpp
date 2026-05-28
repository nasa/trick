
#include <iostream>

#include "trick/Executive.hh"
#include "trick/exec_proto.h"
#include "trick/memorymanager_c_intf.h"

/**
@details
-# Save the number of jobs in the scheduler
-# Copy all of the job information in a checkpointable array
*/
int Trick::Executive::checkpoint() {

    unsigned int ii ;

    /*
       The all_jobs_vector contains memory that is not known to the memory manager.  We need to
       copy the information into memory that is declared to the memory manager.
     */
    /* save the number of jobs in the scheduler */
    num_all_jobs = all_jobs_vector.size() ;

    /* copy all of the job information in a checkpointable array */
    if ( num_all_jobs > 0 ) {
        /* Free any stale allocation from a previous checkpoint cycle where
           post_checkpoint() was not called (e.g. when this checkpoint() fires while
           all_jobs_for_checkpoint was not cleaned up). */
        if ( all_jobs_for_checkpoint != NULL ) {
            TMM_delete_var_a(all_jobs_for_checkpoint) ;
            all_jobs_for_checkpoint = NULL ;
        }
        all_jobs_for_checkpoint = (Trick::JobData *)TMM_declare_var( TRICK_STRUCTURED, "Trick::JobData",
         0 , "all_jobs" , 1 , (int *)&num_all_jobs) ;
        /* If TMM_declare_var returned NULL, "all_jobs" is already in the MemoryManager
           (all_jobs_for_checkpoint was NULL but the MM entry persists, e.g. after a
           checkpoint restore where pointer did not update all_jobs_for_checkpoint).
           Delete the stale entry by name and retry. */
        if ( all_jobs_for_checkpoint == NULL ) {
            TMM_delete_var_n("all_jobs") ;
            all_jobs_for_checkpoint = (Trick::JobData *)TMM_declare_var( TRICK_STRUCTURED, "Trick::JobData",
             0 , "all_jobs" , 1 , (int *)&num_all_jobs) ;
        }
        if ( all_jobs_for_checkpoint != NULL ) {
            for ( ii = 0 ; ii < num_all_jobs ; ii++ ) {
                all_jobs_for_checkpoint[ii] = *(all_jobs_vector[ii]) ;
            }
        }
    }

    return(0) ;

}
