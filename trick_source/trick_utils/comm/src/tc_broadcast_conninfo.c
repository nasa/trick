
/*
 * Broadcast Connection Information
 */

#ifndef __WIN32__
#include <pthread.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

#ifdef __WIN32__
DWORD WINAPI tc_broadcast_conninfo(LPVOID info)
{

    DWORD ret = 0;
#else
void *tc_broadcast_conninfo(void *info)
{

    void *ret = NULL;
#endif
    BC_INFO *bc_info = (BC_INFO *) info;
    SEND_ME *SM;

    SM = &(bc_info->send_me);
    SM->port = htonl((uint32_t) SM->port);
    SM->pid = htonl((uint32_t) SM->pid);
    SM->disabled = htonl((uint32_t) SM->disabled);
    SM->conn_initiated = htonl((uint32_t) SM->conn_initiated);

    /* Continue broadcasting until the connection device is initialized */
    while (!tc_isValid(bc_info->device) && !bc_info->device->port) {

        sendto(bc_info->fd,
               (char *) &bc_info->send_me, sizeof(SEND_ME), 0,
               (struct sockaddr *) &bc_info->addr, (socklen_t) sizeof(bc_info->addr));
#ifdef __WIN32__
        Sleep(200);
#else
        usleep(200000);
#endif
    }

    free(info);
#ifdef __WIN32__
    ExitThread(0);
#else
    pthread_exit(ret);
#endif

    return (ret);
}
