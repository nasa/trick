
/* 
 * $Id: Executive_loop.cpp 2919 2013-04-16 18:49:02Z alin $
 */

#include <stdlib.h>

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Executive/include/Exec_exception.hh"

/**
@details
-# If the #freeze_command flag is true, set the simulation mode to Freeze.
-# The scheduler sets the mode to Run.
   Requirement  [@ref r_exec_mode_1]
-# The scheduler calls Executive::loop_single_thread if only one thread of execution is present.
   Requirement  [@ref r_exec_mode_0]
-# The scheduler calls Executive::loop_multi_thread() if more than one thread of
   execution is present.
   Requirement  [@ref r_exec_mode_1]
-# If an exception is caught, the return code, file where the exception occured, and the
   reason for the exception is saved.  The exception return code is returned.
-# If an unknown exception is caught, set the exception return code to -1, the file to unknown, and the
   reason for the exception as unknown.  The exception return code is returned.
-# If no execption is caught return 0
*/
int Trick::Executive::loop() {

    try {

        /* If no threads are present, the simulation did not initialize correctly, return */
        if ( threads.size() == 0 ) {
            return(0) ;
        }

        /* If the #freeze_command flag is true, set the simulation mode to Freeze. */
        if ( freeze_command ) {
            exec_command = FreezeCmd ;
        }

        /* Set the simulation mode to Run. */
        mode = Run ;

        if ( threads.size() == 1 ) {
            /* Call loop_single_thread() if there is a single thread. */
            loop_single_thread() ;
        } else {
            /* Call loop_multi_thread() if there is more than one thread. */
            loop_multi_thread() ;
        }
    }
    catch (Trick::Exec_exception & ex ) {
        /* Handle exception type Trick::Exec_exception.  Set the file name and error message.
           Return the exception return code. */
        except_return = ex.ret_code ;
        except_file = ex.file ;
        except_message = ex.message ;
        return(ex.ret_code) ;
    } catch (const std::exception &ex) {
        if ( curr_job != NULL ) {
            except_file = curr_job->name ;
        } else {
            except_file = "somewhere in Executive::run" ;
        }
        fprintf(stderr, "\nExecutive::loop terminated with std::exception\n  ROUTINE: %s\n  DIAGNOSTIC: %s\n"
         "  STOP TIME: %f\n" , except_file.c_str() , ex.what() , get_sim_time()) ;
        exit(-1) ;
    } catch (...) {
        /* Handle unknown exceptions.  Set the file name and error message to unknown. exit -1. */
        if ( curr_job != NULL ) {
            except_file = curr_job->name ;
        } else {
            except_file = "somewhere in Executive::run" ;
        }
        except_message = "unknown error" ;
        fprintf(stderr, "\nExecutive::loop terminated with unknown exception\n  ROUTINE: %s\n  DIAGNOSTIC: %s\n"
         "  STOP TIME: %f\n" , except_file.c_str() , except_message.c_str() , get_sim_time()) ;
        exit(-1) ;
    }

    /* return 0 if there are no errors. */
    return(0) ;
}

