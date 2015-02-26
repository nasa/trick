/*******************************************************************************
PURPOSE: (Scheduled job.)
*******************************************************************************/

#include "../include/test4.h"
#include "sim_services/include/exec_proto.h"

int sched_job_2(TEST4* T) {
    double time = exec_get_sim_time();

    T->inv_exp_t = 1.0 / exp( time);
    printf("%f: sched_job_2() called.\n", time);
    return (0);
}
