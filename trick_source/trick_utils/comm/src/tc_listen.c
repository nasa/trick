
/*
 * Check to see if a client is attempting to connect
 */

#include "trick/tc.h"
#include "trick/tc_proto.h"

#if (__APPLE__ | __WIN32__| __Lynx__ | __QNX__ | __ghs )

#include <sys/types.h>
#ifdef __APPLE__
#include <sys/time.h>
#include <unistd.h>
#endif

int tc_listen(TCDevice * listen_device)
{
    int status;

    fd_set chkset;
    struct timeval zero_time = { 0, 0 };

    /* Clear check file descriptor set */
    FD_ZERO(&chkset);

    /* Place device file descriptor into check file desciptor set */
    FD_SET(listen_device->socket, &chkset);

    /* Determine if the file descriptor has data to read */
    status = select(listen_device->socket + 1, &chkset, (fd_set *) NULL, (fd_set *) NULL, &zero_time);

    return (status);

}

#else

#include <poll.h>

int tc_listen(TCDevice * listen_device)
{
    int status;

    struct pollfd pfd;

    pfd.fd = listen_device->socket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    status = poll(&pfd, (nfds_t) 1, 0);

    return (status);

}
#endif
