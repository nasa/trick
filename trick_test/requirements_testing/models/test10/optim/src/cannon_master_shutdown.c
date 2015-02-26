#include "../../aero/include/cannon_aero.h"

int cannon_master_shutdown(CANNON_AERO *C) {
    C->master_shutdown = 1;
    printf("cannon_master_shutdown called \n");
    return 0;
}
