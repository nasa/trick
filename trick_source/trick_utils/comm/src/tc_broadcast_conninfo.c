/******************************************************************************
*                                                                             *
* Trick Simulation Environment Software                                       *
*                                                                             *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                     *
* All rights reserved.                                                        *
*                                                                             *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized   *
* use of Trick Software including source code, object code or executables is  *
* strictly prohibited and LinCom assumes no liability for such actions or     *
* results thereof.                                                            *
*                                                                             *
* Trick Software has been developed under NASA Government Contracts and       *
* access to it may be granted for Government work by the following contact:   *
*                                                                             *
* Contact: Charles Gott, Branch Chief                                         *
*          Flight Robotic Systems Branch                                      *
*          Automation, Robotics, & Simulation Division                        *
*          NASA, Johnson Space Center, Houston, TX                            *
*                                                                             *
******************************************************************************/

/* 
 *  $Id: tc_broadcast_conninfo.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 * Broadcast Connection Information 
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

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
