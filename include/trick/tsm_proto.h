/*
PURPOSE:
     (Specify the trick shared memory library API and structure definition)
PROGRAMMERS:
     (((Danny Strauss) (L3) (Sep 2012) (--) (Initial Version)))
ICG:
     (No)
*/

#ifndef TSM_PROTO_H
#define TSM_PROTO_H

#include "trick/tsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize a shared memory segment */
int tsm_init(TSMDevice * device);

/* Initialize a shared memory segment containing a pthread lock */
int tsm_init_with_lock(TSMDevice * device);

/* Reconnect to a disconnected shared memory segment */
int tsm_reconnect(TSMDevice * device);

/* Disconnect (detach) a shared memory segment */
int tsm_disconnect(TSMDevice * device);

#ifdef __cplusplus
}
#endif
#endif
