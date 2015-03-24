/*
PURPOSE:
     (Specify the trick shared memory library API and structure definition)
PROGRAMMERS:
     (((Danny Strauss) (L3) (Sep 2012) (--) (Initial Version)))
*/

#ifndef TSM_H
#define TSM_H

#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <pthread.h>

/* Status codes */
#define TSM_SUCCESS                     0
#define TSM_FAIL                        1

/* Set default for timeout limit high */
#define TSM_MAX_TIMEOUT_LIMIT   1.0e20

#ifdef STAND_ALONE
#define send_hs fprintf
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

    int size;                          /* -- Size of shared memory segment */
    void* addr;                        /* ** Address of shared memory segment */

    char key_file[256];                 /* -- An existing file name to use in generating key (ftok) for shared memory */
    int  key_id;                       /* ** Id value to use in generating key value (ftok) for shared memory */
    key_t key;                         /* ** The key returned from ftok call, needed for shared memory create (shmget) */
    int shmid;                         /* ** The shared memory id returned from shmget call */

    double timeout_limit;              /* -- s Optional: how long do we wait for shared memory data before we time out */
    int default_val;                   /* --   Optional: value to set shared memory data to after it's been read */

    char use_lock;                     /* --   Optional: if true then tsm_init creates pthread read/write lock */
    pthread_rwlockattr_t rwlattr;      /* **   Optional: pthread read/write lock attributes */
    pthread_rwlock_t* rwlock_addr;     /* **   Optional: Address of pthread lock in shared memory segment */

} TSMDevice;

#ifdef __cplusplus
}
#endif

#endif
