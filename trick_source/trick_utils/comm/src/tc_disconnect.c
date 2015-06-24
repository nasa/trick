
/*
 * Disconnect a device
 */

#ifndef __WIN32__
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

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
