
/*
 * Read data from a device
 */

#ifndef __WIN32__
#include <errno.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_read_(TCDevice * device, char *buffer, int size, const char *file, int line)
{
    long nbytes = 0;

    char client_str[TC_TAG_LENGTH + 20];
    long tmp_nbytes = 0;
    long tmp_size = (long) size;
    void *data = (void *) buffer;
    double ref_time = 0;
    double delta = 0;
    int error = TC_SUCCESS;
    char error_str[512];

    // To support UDP
    unsigned int cliLen = sizeof(struct sockaddr_in);

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, file, line, "tc_read: Trying to read from a NULL device");
        return (-1);
    }

    if (device->disabled) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, file, line, "tc_read: Trying to read from a disabled device");
        return (-1);
    }

    if (device->socket == TRICKCOMM_INVALID_SOCKET) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, file, line, "tc_read: Trying to read from an invalid socket");
        return (-1);
    }

    sprintf(client_str, "(ID = %d  tag = %s)", device->client_id, device->client_tag);

    trick_error_report(device->error_handler, TRICK_ERROR_ALL, file, line,
                       "tc_read: %s reading %d bytes\n", client_str, size);

    /* If this is a software blocking read get the current time from the system */
    if (device->blockio_type == TC_COMM_TIMED_BLOCKIO) {
        ref_time = tc_clock_init();
    }

    /*
     * Spin until all requested data is
     * available, an error is returned from recv, or a timeout
     * limit has been reached. This code is needed for the
     * socket because sockets can return less
     * data than what was requested when data packets
     * cross block boundries.
     */
    while (nbytes != size) {

        /*
         * recvfrom will return -1 with tc_errno set to TC_EWOULDBLOCK
         * if this is a non-blocking socket (blockio = NOBLOCKIO)
         * and no data is available.  recvfrom will usually return 0
         * for a broken connection.
         */
        while ((tmp_nbytes = recvfrom(device->socket, data, (size_t) tmp_size,
                                      TC_NOSIGNAL, (struct sockaddr *) &device->cliAddr, &cliLen)) < 0
               && tc_errno == TRICKCOMM_EINTR);

        /* if tmp_nbytes == 0, that is a broken pipe, break out */
        if (tmp_nbytes == 0) {
            trick_error_report(device->error_handler, TRICK_ERROR_ALERT, file, line,
                               "tc_read: %s Other side disconnected. (recvfrom returned 0)", client_str);
            tc_disconnect(device);
            return (nbytes);
        } else if (tmp_nbytes == -1) {
            error = tc_errno;
            if (error != TRICKCOMM_EAGAIN && error != TRICKCOMM_EWOULDBLOCK) {
                sprintf(error_str, "tc_read: %s %s (tc_errno = %d)", client_str, strerror(error), error);
                trick_error_report(device->error_handler, TRICK_ERROR_ALERT, file, line, error_str);
                tc_disconnect(device);
                return (nbytes);
            }
        }
        /* Keep track of total number of bytes read */
        else if (tmp_nbytes > 0) {

#if 0
            /* This indicates the socket protocol: SOCK_STREAM (1) or SOCK_DGRAM (2). */
            int proto_for_socket;
            int opt_len = 4;
            int retval;

            /* Get the socket protocol */
            retval = getsockopt(device->socket, SOL_SOCKET, SO_TYPE,
                                (void *) &proto_for_socket, (socklen_t *) & opt_len);
            if (retval < 0) {
                perror("tc_read: getsockopt");

                trick_error_report(device->error_handler,
                                   TRICK_ERROR_ALERT, file, line,
                                   "Couldn't get the socket protocol. Assumed TCP/IP (not UDP)");
                proto_for_socket = SOCK_STREAM;
            }
#endif

            nbytes += tmp_nbytes;
            /* For UDP (SOCK_DGRAM) just return data with whatever number of bytes were received. */
            if (device->socket_type == SOCK_DGRAM)
                break;

        }

        /* Adjust data pointers for partial reads */
        if (tmp_nbytes > 0 && tmp_nbytes != tmp_size) {
            data = (void *) ((char *) data + tmp_nbytes);
            tmp_size -= tmp_nbytes;
        }

        if (device->blockio_type == TC_COMM_TIMED_BLOCKIO) {

            delta = tc_clock_time(ref_time);

            /* Check for timeouts; this prevents hanging here if the writer dies */
            if (device->blockio_limit < delta && nbytes != size) {
                error = TC_READWRITE_TIMEOUT;
                break;
            }

        } else if (device->blockio_type == TC_COMM_ALL_OR_NOTHING) {

            /* If nothing read and nothing pending break out */
            if (nbytes == 0 && tmp_nbytes == -1 && (tc_errno == TRICKCOMM_EWOULDBLOCK || tc_errno == TRICKCOMM_EAGAIN)) {
                nbytes = -1;
                error = TC_EWOULDBLOCK;
                break;
            }

            /* If something read release processor and loop back for more */
            else if (tmp_nbytes == -1 && tc_errno == TRICKCOMM_EWOULDBLOCK) {
                /* Yield the processor so queued proceses may run */
                TC_RELEASE();
            }
        } else if (device->blockio_type == TC_COMM_NOBLOCKIO) {
            if (tmp_nbytes == -1 && (tc_errno == TRICKCOMM_EWOULDBLOCK || tc_errno == TRICKCOMM_EAGAIN)) {
                if (nbytes == 0) {
                    nbytes = -1;
                }
                error = TC_EWOULDBLOCK;
                break;
            }
        }

    }

    /* If read doesn't capture all data, handle it. */
    switch (error) {
        case TC_READWRITE_TIMEOUT:
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ADVISORY, file, line,
                               "tc_read: %s Failed to read within the specified "
                               "time limit of %f seconds. delta = %f", client_str, device->blockio_limit, delta);
            break;
        case TC_EWOULDBLOCK:
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALL, file, line,
                               "tc_read: %s %d of %d bytes read during "
                               "non-blocking read.", client_str, nbytes, size);
            break;
        case TC_SUCCESS:
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALL, file, line,
                               "tc_read: %s: %d bytes successfully read\n", client_str, nbytes);
            break;
    }

    return ((int) nbytes);

}
