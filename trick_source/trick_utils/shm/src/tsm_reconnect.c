/*
 * Reconnect to a disconnected shared memory segment - get it and attach to it
 */
#include "trick/tsm.h"
#include "trick/tsm_proto.h"

int tsm_reconnect(TSMDevice * shm_device)
{

    if (shm_device->size <= 0) {
        perror("tsm_init SHARED MEMORY RECONNECT FAILED (invalid size)");
        fprintf(stderr, "size=%d\n", shm_device->size);
        return (TSM_FAIL);
    }

    if (shm_device->key <= 0) {
        perror("tsm_init SHARED MEMORY RECONNECT FAILED (invalid key)");
        fprintf(stderr, "key=%d\n", shm_device->key);
        return (TSM_FAIL);
    }

    // Create the shared memory segment with universal read priveleges
    // If this segment already exists, this does nothing
    shm_device->shmid = shmget(shm_device->key, shm_device->size, 0666 | IPC_CREAT);
    if (shm_device->shmid == -1) {
        perror("tsm_init SHARED MEMORY RECONNECT FAILED (shmget)");
        fprintf(stderr, "size=%d\n", shm_device->size);
        return (TSM_FAIL);
    }

    // Attach to the created shared memory segment
    shm_device->addr = shmat(shm_device->shmid, (void *)0, 0);
    if (shm_device->addr == (void*)-1) {
        perror("tsm_init SHARED MEMORY RECONNECT FAILED (shmat)");
        return (TSM_FAIL);
    }

    // Reassign the lock pointer
    if (shm_device->use_lock) {
        shm_device->rwlock_addr = (pthread_rwlock_t*)(shm_device->addr + shm_device->size - sizeof(pthread_rwlock_t));
    }

    fprintf(stderr, "Trick shared memory reconnect Successful (shmid=%d)\n", shm_device->shmid);

    return (TSM_SUCCESS);
}
