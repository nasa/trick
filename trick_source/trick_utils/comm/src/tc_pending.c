
/*
 * See if data is available for reading on a non-blocking connection
 */

#ifndef __WIN32__
#  include <sys/ioctl.h>
#  define IOCTL_SOCKET ioctl
#else
#  define IOCTL_SOCKET ioctlsocket
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_pending(TCDevice * device)
{
    int n_read;

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to call tc_pending on a NULL device");
        return (-1);
    }

    /* Use ioctl to check the number of bytes to read */
    IOCTL_SOCKET(device->socket, (unsigned long) FIONREAD, &n_read);

    return (n_read);

}
