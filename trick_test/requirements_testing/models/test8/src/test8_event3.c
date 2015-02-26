/*******************************************************************************
PURPOSE: (Test CALLs from ACTION blocks in events defined in the input file.
*******************************************************************************/

#include "sim_services/include/exec_proto.h"

int test8_event3() {
	printf("event3 (%f): This event fires every other second.\n", exec_get_sim_time());
}
