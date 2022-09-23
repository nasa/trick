/*
 * Initialize a shared memory segment containing a pthread read/write lock
 * and initialize the lock to be shared amongst multiple processes.
 * The user can then use it to lock memory during read/write operations.
 */
#include "trick/tsm.h"
#include "trick/tsm_proto.h"


int tsm_init_with_lock(TSMDevice * shm_device)
{
    int ret;

    shm_device->use_lock = 1;
    ret = tsm_init(shm_device);
    return (ret);

}
