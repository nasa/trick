#include "../../aero/include/cannon_aero.h"

int cannon_slave_shutdown(CANNON_AERO *C) {
   C->slave_shutdown = 1;
    printf("cannon_slave_shutdown called\n");
    return 0;
}
