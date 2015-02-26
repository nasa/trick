#include "../include/test8.h"
#include "sim_services/include/exec_proto.h"

int test8_sched(TEST8 *T) {
	if (exec_get_sim_time() < 19) {
		T->i++;
	}
	else {
		T->i--;
	}
	//printf("%f: i = %d\n", exec_get_sim_time(), T->i);
}
