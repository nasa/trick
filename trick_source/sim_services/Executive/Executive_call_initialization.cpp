
#include <iostream>
#include <sys/resource.h>

#include "trick/Executive.hh"
#include "trick/ExecutiveException.hh"

/**
@details
-# Call the initialization jobs.
   -# The scheduler will immediately return if the initialization job returns a
      non-zero return code.  The return code of the initialization job will be
      returned from Trick::Executive::init().
-# If no execption is thrown return 0
*/
int Trick::Executive::call_initialization() {

    int ret = 0 ;

    except_file = "in intialization queue" ;

    /* Call the initialization jobs. */
    initialization_queue.reset_curr_index() ;
    while ( (curr_job = initialization_queue.get_next_job()) != NULL ) {
        ret = curr_job->call() ;
        if ( ret != 0 ) {
            throw Trick::ExecutiveException(ret , curr_job->name.c_str() , 0 , "initialization job did not return 0") ;
        }
    }

    /* return 0 if there are no errors. */
    return ret ;
}
