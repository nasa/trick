
/*
 * Disconnect (detach) a shared memory segment
 */
#include "trick/tsm.h"
#include "trick/tsm_proto.h"


int tsm_disconnect(TSMDevice * shm_device)
{
    int ret;

    ret = TSM_SUCCESS;

    // if we had a read/write lock, destroy it first
    if (shm_device->rwlock_addr != NULL) {
        ret = pthread_rwlockattr_destroy(&shm_device->rwlattr);
        if (ret == -1) {
            perror("tsm_disconnect SHARED MEMORY DESTROY LOCK ATTRIBUTES FAILED");
        }
        ret = pthread_rwlock_destroy(shm_device->rwlock_addr);
        if (ret == -1) {
            perror("tsm_disconnect SHARED MEMORY DESTROY LOCK FAILED");
        }
    }

    ret = shmdt(shm_device->addr);
    if (ret == -1) {
        perror("tsm_disconnect SHARED MEMORY DETACH FAILED");
    }

    return (ret);
}
