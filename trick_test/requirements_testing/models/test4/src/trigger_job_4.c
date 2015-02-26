/*******************************************************************************
PURPOSE: (Malfunction trigger.)
*******************************************************************************/

#include "../include/test4.h"
#include "sim_services/include/exec_proto.h"

int trigger_job_4(TEST4* T) {
	double time = exec_get_sim_time();
	printf("%f: trigger_job_4 called: returned ", time);
	if (time >= T->start_time && time <= T->stop_time) {
		printf("true\n");
		return 1;
	}
	printf("false\n");
	return 0;
}
