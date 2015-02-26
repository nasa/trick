/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test sched_automatic_last)
***************************************************************************/
#include "../include/sched.h"
#include <stdio.h>
#include "sim_services/include/executive.h"
#include "sim_services/include/jobdata.h"
#include "sim_services/include/module_classes.h"
#include "sim_services/include/exec_proto.h"

Realtime_Process_ID exec_get_process_id(void);

extern JOBDATA *exec_jobs[];

int sched_automatic_last(
                            /* RETURN: -- Always return zero */
  SCHEDULE *S)              /* INOUT:  -- Parameter */

{

	EXECUTIVE *E;
	EXEC_WORK *EW;

	Realtime_Process_ID process_id;
	
	E = exec_get_exec();
	EW = &(E->work);

	process_id = exec_get_process_id();
	exec_reschedule(3, EW->job_select[process_id], exec_get_sim_time() + 1.2);

	return(0) ; 
}
