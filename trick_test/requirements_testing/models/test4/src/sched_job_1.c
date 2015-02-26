/*******************************************************************************
PURPOSE: (Scheduled job.)
*******************************************************************************/

#include <math.h>
#include "../include/test4.h"
#include "sim_services/include/exec_proto.h"

int sched_job_1(TEST4* T) {
    double time = exec_get_sim_time();

    T->cos_pi_prev_t = T->cos_pi_t;
    T->cos_pi_t = cos( (3.14159265) * time);

    printf("%f: sched_job_1() called.\n", time);

}
