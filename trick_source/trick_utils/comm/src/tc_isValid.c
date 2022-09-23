
/*
 * Check to see if a device is valid
 */

#ifndef __WIN32__
#include <errno.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_isValid(TCDevice * device)
{
    int nread;
    char one_byte;
    int ret = 0;

    if (device && (device->socket != TRICKCOMM_INVALID_SOCKET)) {

        tc_errno = 0;
        do {
            nread = recv(device->socket, &one_byte, sizeof(char),
                         MSG_PEEK | MSG_DONTWAIT);
        } while ((nread == -1) && (tc_errno == TRICKCOMM_EINTR));

        /* Return 0 or 1 based on the response from recv() */
        if ((nread <= 0) && (tc_errno != TRICKCOMM_EAGAIN)) {

            ret = 0; /* The socket is not valid */
        }
        else {

            ret = 1; /* The socket is valid */
        }
    }

    return (ret);

}/* end tc_isValid */
