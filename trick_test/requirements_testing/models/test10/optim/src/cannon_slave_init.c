#include "../../aero/include/cannon_aero.h"

int cannon_slave_init(CANNON_AERO *C) {
    C->slave_init = 1;
    printf("cannon_slave_init called\n");
    return 0;
}
