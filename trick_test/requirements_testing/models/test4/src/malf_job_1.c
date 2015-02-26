/*******************************************************************************
PURPOSE: (Malfunction.)
*******************************************************************************/

#include "../include/test4.h"
#include "sim_services/include/exec_proto.h"

int malf_job_1(TEST4* T) {
    double time = exec_get_sim_time();

    printf("%f: ############################### malf_1_job() called.\n", time);
    return (0);
}
