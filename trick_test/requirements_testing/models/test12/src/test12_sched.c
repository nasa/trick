#include "../include/test12.h"
#include "sim_services/include/exec_proto.h"

int test12_sched(TEST12 *T) {
        T->i = sin(exec_get_sim_time() * 3.14);
}
