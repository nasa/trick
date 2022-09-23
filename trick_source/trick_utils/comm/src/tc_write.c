
/*
 * Write data to a device
 */

#ifndef __WIN32__
#include <errno.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_write_(TCDevice * device, char *buffer, int size, const char *file, int line)
{
    char client_str[TC_TAG_LENGTH + 256];
    int nbytes = 0;
    int tmp_nbytes = 0;
    void *tmp_data = (void *) buffer;
    int tmp_size = size;
    double ref_time = 0;
    double delta = 0;
    int error = TC_SUCCESS;
    char error_str[512];

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr, TRICK_ERROR_ALERT, file, line, "Trying to write to a NULL device");
        return (-1);
    }

    if (device->disabled) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, file, line, "Trying to write to a disabled device");
        return (-1);
    }

    if (device->socket == TRICKCOMM_INVALID_SOCKET) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, file, line, "Trying to write to an invalid socket");
        return (-1);
    }

    sprintf(client_str, "(ID = %d  tag = %s)", device->client_id, device->client_tag);
    trick_error_report(device->error_handler, TRICK_ERROR_ALL, file, line, "%s writing %d bytes\n", client_str, size);

    /* If this is a software blocking write get the current time from the system */
    if (device->blockio_type == TC_COMM_TIMED_BLOCKIO) {
        ref_time = tc_clock_init();
    }

    while (nbytes != size) {

        /* Send will return -1 with tc_errno set to EINTR if it was interrupted by the delivery of a signal.  Re-send
           data if this occurs. */
        while ((tmp_nbytes = sendto(device->socket, tmp_data,
                                    (size_t) tmp_size, TC_NOSIGNAL,
                                    (struct sockaddr *) &device->remoteServAddr,
                                    (socklen_t) sizeof(struct sockaddr_in))) < 0 && tc_errno == TRICKCOMM_EINTR);

        /* If send had an error return nbytes to indicate broken connection */
        if (tmp_nbytes < 0) {
            error = tc_errno;
            if (error != TRICKCOMM_EAGAIN && error != TRICKCOMM_EWOULDBLOCK) {
                sprintf(error_str, "tc_write: %s %s (tc_errno = %d)", client_str, strerror(error), error);
                trick_error_report(device->error_handler, TRICK_ERROR_ALERT, file, line, error_str);
                tc_disconnect(device);
                return (nbytes);
            }
        }

        /* Keep track of total number of bytes writes */
        else if (tmp_nbytes > 0) {
            nbytes += tmp_nbytes;
        }

        /* Adjust data pointers for partial writes */
        if (tmp_nbytes > 0 && tmp_nbytes != tmp_size) {
            tmp_data = (void *) ((char *) tmp_data + tmp_nbytes);
            tmp_size -= tmp_nbytes;
        }

        if (device->blockio_type == TC_COMM_TIMED_BLOCKIO) {

            delta = tc_clock_time(ref_time);
            /* Check for timeouts; this prevents hanging here if the reader dies */
            if (device->blockio_limit < delta) {
                error = TC_READWRITE_TIMEOUT;
                break;
            }

            if (tmp_nbytes == -1 && tc_errno == TRICKCOMM_EWOULDBLOCK) {
                /* Yield the processor so queued proceses may run */
                TC_RELEASE();
            }
        } else if (device->blockio_type == TC_COMM_ALL_OR_NOTHING) {
            /* If nothing read and nothing pending break out */
            if (nbytes == 0 && tmp_nbytes == -1 && (tc_errno == TRICKCOMM_EWOULDBLOCK || tc_errno == TRICKCOMM_EAGAIN)) {
                error = TC_EWOULDBLOCK;
                nbytes = -1;
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

    /*
     * If write didn't write all bytes, handle it
     */
    switch (error) {
        case TC_READWRITE_TIMEOUT:
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ADVISORY, file, line,
                               "%s Failed to write within the specified "
                               "time limit of %f seconds. delta = %f ref_time = %f", client_str, device->blockio_limit, delta, ref_time);
            break;
        case TC_EWOULDBLOCK:
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALL, file, line,
                               "%s No data written during non-blocking write.", client_str);
            break;
        case TC_SUCCESS:
            trick_error_report(device->error_handler, TRICK_ERROR_ALL,
                               file, line, "%s: %d bytes successfully written\n", client_str, nbytes);
            break;
    }

    return (nbytes);

}
