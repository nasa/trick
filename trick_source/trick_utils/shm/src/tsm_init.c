
/*
 * Initialize a shared memory segment - get it and attach to it
 */
#include "trick/tsm.h"
#include "trick/tsm_proto.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

key_t my_ftok(const char* path, int proj_id) ;

key_t my_ftok(const char* path, int proj_id) {
    struct stat st;
    char size_8bit;
    if (stat(path, &st) == -1) {
        perror("tsm_init SHARED MEMORY CREATE FAILED (stat call failed)");
        fprintf(stderr, "key file=%s\n", path);
        return (-1) ;
    }
    // the linux ftok() call returns this 32 bit value:
    //    pppp pppp dddd dddd iiii iiii iiii iiii
    // where p= proj_id, d=device, i=inode
    //return ((key_t)( (proj_id<<24) + (long)(st.st_dev<<16) + (long)(st.st_ino & 0xFFFF) ));
    // however, the device (st_dev) is not necessarily the same from machine to machine
    // which means if you run a checkpointed sim on a different machine, the shared memory
    // keys would be different and therefore would not work.
    // So we will do our own key generation using st_size instead of st_dev:
    size_8bit = st.st_size;
    return ((key_t)( (proj_id<<24) + (long)(size_8bit<<16) + (long)(st.st_ino & 0xFFFF) ));

}


int tsm_init(TSMDevice * shm_device)
{

    // set proj_id to 1..255 to create 255 different keys
    // 255 is therefore the max number of shared mem segments we can create
    static int proj_id = 1;

    if (shm_device->size <= 0) {
        perror("tsm_init SHARED MEMORY CREATE FAILED (invalid size)");
        fprintf(stderr, "size=%d\n", shm_device->size);
        return (TSM_FAIL);
    }

    // use_lock means we need to create a pthread read/write lock in shared memory
    // we will stick the lock onto the end of shared memory segment
    if (shm_device->use_lock) {
        shm_device->size += sizeof(pthread_rwlock_t);
    }

    // Create a key for call to shmget, by passing an existing file name and proj_id to ftok()
    // If user has not set the filename, use this file as the default
    if (proj_id > 255) {
        perror("tsm_init SHARED MEMORY CREATE FAILED (exceeded max shared memory segments of 255)");
        fprintf(stderr, "proj_id=%d\n", proj_id);
        return (TSM_FAIL);
    }
    if (shm_device->key_file[0] == '\0') {
        sprintf(shm_device->key_file, "%s/trick_source/trick_utils/shm/src/tsm_init.c", getenv("TRICK_HOME"));
    }
    //shm_device->key = ftok(shm_device->key_file, proj_id);
    // we will use our own key generation in my_ftok
    shm_device->key = my_ftok(shm_device->key_file, proj_id);
    if (shm_device->key == -1) {
        perror("tsm_init SHARED MEMORY CREATE FAILED (ftok)");
        fprintf(stderr, "filename=%s\n", shm_device->key_file);
        return (TSM_FAIL);
    }

    // Create the shared memory segment with universal read priveleges
    // If this segment already exists, this does nothing
    shm_device->shmid = shmget(shm_device->key, shm_device->size, 0666 | IPC_CREAT);
    if (shm_device->shmid == -1) {
        perror("tsm_init SHARED MEMORY CREATE FAILED (shmget)");
        fprintf(stderr, "size=%d\n", shm_device->size);
        return (TSM_FAIL);
    }

    // Attach to the created shared memory segment
    shm_device->addr = shmat(shm_device->shmid, (void *)0, 0);
    if (shm_device->addr == (void*)-1) {
        perror("tsm_init SHARED MEMORY CREATE FAILED (shmat)");
        return (TSM_FAIL);
    }

    // Create lock
    if (shm_device->use_lock) {
        shm_device->rwlock_addr = (pthread_rwlock_t*)(shm_device->addr + shm_device->size - sizeof(pthread_rwlock_t));
        // initialize lock attributes
        pthread_rwlockattr_init(&shm_device->rwlattr);
        // set the attribute so lock can be shared amongst processes
        pthread_rwlockattr_setpshared(&shm_device->rwlattr, PTHREAD_PROCESS_SHARED);
        // initialize the lock
        pthread_rwlock_init(shm_device->rwlock_addr, &shm_device->rwlattr);
    }

    fprintf(stderr, "Trick shared memory %d init Successful (shmid=%d)\n", proj_id, shm_device->shmid);

    proj_id++;

    return (TSM_SUCCESS);
}
