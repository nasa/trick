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
 *  $Id: tc_disconnect.c 2898 2013-04-04 15:10:53Z alin $
 */

/*
 * Disconnect a device
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_disconnect(TCDevice * device)
{
    char client_str[TC_TAG_LENGTH + 256];

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to disconnect a NULL device");
        return (TC_DRIVER_ALREADY_DISCONNECTED);
    }


    /* Status message */
    sprintf(client_str, "(ID = %d  tag = %s)", device->client_id, device->client_tag);

    trick_error_report(device->error_handler, TRICK_ERROR_TRIVIAL, __FILE__, __LINE__, "%s \n", client_str);

    shutdown(device->socket, 2);
    CLOSE_SOCKET(device->socket);

    device->socket = TRICKCOMM_INVALID_SOCKET ;
#ifdef __WIN32__

    /* Terminate use of the winsock DLL */
    if (WSACleanup() != 0) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "%s: WSAStartup failed with error %d\n", client_str, WSAGetLastError());
        MessageBox(NULL, "Error", "tc_StartWinSock", MB_OK);
        return (TC_DRIVER_ALREADY_INSTALLED);
    }
#endif

    return (TC_SUCCESS);
}
