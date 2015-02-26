/*******************************************************************************
PURPOSE: (Scheduled job.)
*******************************************************************************/

#include "../include/test4.h"
#include "sim_services/include/exec_proto.h"

int sched_job_3(TEST4* T) {
    double time = exec_get_sim_time();

    printf("%f: sched_job_3() called.\n", time);
    return (0);
}
