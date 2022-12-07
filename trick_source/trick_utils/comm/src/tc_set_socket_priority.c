
/*
 * Set the priority of a socket.
 */

#ifndef __APPLE__

#ifndef __WIN32__
#include <errno.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

int tc_set_socket_priority_(TCDevice * listen_device, TCDevice * device, int priority, const char *file, int line)
{
    char client_str[TC_TAG_LENGTH + 256];

    snprintf(client_str, sizeof(client_str), "(ID = %d  tag = %s)", listen_device->client_id, listen_device->client_tag);

    if (setsockopt(device->socket, SOL_SOCKET, SO_PRIORITY, &priority, sizeof(priority)) < 0) {
        trick_error_report(listen_device->error_handler,
                           TRICK_ERROR_ALERT, file,
                           line, "tc_set_socket_priority: %s: error on set socket option: " "TC_COULD_NOT_SET_SOCKET_PRIORITY", client_str);
        return (TC_COULD_NOT_SET_SOCKET_PRIORITY);
    }

    return(TC_SUCCESS);

}

#endif
