
/*
 * Multicast connect
 */

#include <string.h>
#ifdef __WIN32__
#include <process.h>
#else
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#endif
#include <pthread.h>

#include "trick/tc.h"
#include "trick/tc_proto.h"

int tc_multiconnect(TCDevice * dev_ptr, char *my_tag, char *other_tag, TrickErrorHndlr * not_used)
{
    int status;
    int num_try = 0;
    TCDevice *server_device;
    char name[80];

    int size;
    char *cptr;

    /* Multicast vars */
    struct sockaddr_in addr;
    int fd, nbytes;
    struct ip_mreq mreq;
    BC_INFO bc_info, *bc_copy;
    SEND_ME read_me;
    int i_am_client;
    int value;
    int num_attempts = 10;
    int found_conn;
#ifdef __WIN32__
    HANDLE thread;
    DWORD threadId;
    int curr_pid;
    WSADATA wsaData;
#else
    pthread_t thread;
    pid_t curr_pid;
#endif

    (void) not_used;                   /* unused */

    if (!dev_ptr) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr, TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to connect a NULL device");
        return (-1);
    }

    if (dev_ptr->disabled != 0) {
        trick_error_report(dev_ptr->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "[33m%s| cannot connect disabled device %s[00m\n", my_tag, other_tag);
        return (TC_SUCCESS);
    }

    trick_error_report(dev_ptr->error_handler,
                       TRICK_ERROR_ADVISORY, __FILE__, __LINE__,
                       "[36m%s| multiconnecting to %s[00m\n", my_tag, other_tag);

    /* Create a TrickComm listen device Determine port for listen device */
    server_device = (TCDevice *) malloc(sizeof(TCDevice));
    memset((void *) server_device, '\0', sizeof(TCDevice));
    server_device->error_handler = dev_ptr->error_handler;

    server_device->port = 0;
    if ((status = tc_init(server_device)) != TC_SUCCESS) {
        trick_error_report(server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "could not open listen device!\n");
        return (status);
    }

    /*
     * Initialize Broadcast Info Structure
     */
    gethostname(name, (size_t) 80);

    strcpy(bc_info.send_me.addr, name);
    bc_info.send_me.port = server_device->port;
    bc_info.send_me.pid = curr_pid = getpid();
    bc_info.send_me.conn_initiated = 0;

    if (my_tag != (char *) NULL) {
        strncpy(bc_info.send_me.my_tag, my_tag, (size_t) TC_TAG_LENGTH);
        bc_info.send_me.my_tag[TC_TAG_LENGTH - 1] = '\0';
    } else {
        bc_info.send_me.my_tag[0] = '\0';
    }

    if (other_tag != (char *) NULL) {
        strncpy(bc_info.send_me.other_tag, other_tag, (size_t) TC_TAG_LENGTH);
        bc_info.send_me.other_tag[TC_TAG_LENGTH - 1] = '\0';
    } else {
        bc_info.send_me.other_tag[0] = '\0';
    }

    bc_info.device = dev_ptr;

#ifdef __WIN32__
    /* Initiate use of the Windows socket DLL */
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        perror("tc_multiconnect: WSAStartup");
        WSACleanup();
        return (TC_COULD_NOT_ACCEPT);
    }
#endif

    /* Create the broadcast socket */
    if ((bc_info.fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("tc_multiconnect socket");
    }

    /* Set up destination address */
    memset(&bc_info.addr, 0, sizeof(bc_info.addr));
    bc_info.addr.sin_family = AF_INET;
    bc_info.addr.sin_addr.s_addr = inet_addr(TC_MULT_GROUP);
    bc_info.addr.sin_port = htons(TC_MULT_PORT);

    /*
     * Create socket to listen for connections
     */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("tc_multiconnect socket");
    }

    value = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &value, (socklen_t) sizeof(value)) < 0) {
        perror("setsockopt: reuseaddr");
    }
#ifdef SO_REUSEPORT
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *) &value, sizeof(value)) < 0) {
        perror("setsockopt: reuseport");
    }
#endif

    /* Set up destination address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(TC_MULT_PORT);

    /* Bind to receive address */
    if (bind(fd, (struct sockaddr *) &addr, (socklen_t) sizeof(addr)) < 0) {
        perror("tc_multiconnect bind");
    }

    /* Use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr = inet_addr(TC_MULT_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    size = sizeof(mreq);
    cptr = (char *) &mreq;
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, cptr, (socklen_t) size) < 0) {
#ifdef __WIN32__
        TCHAR szError[100];     // Error message string

        wsprintf(szError, TEXT("setsockopt failed! Error: %d"), WSAGetLastError());
        MessageBox(NULL, szError, TEXT("Error"), MB_OK);
#endif
        perror("setsockopt: ip_add_membership");
    }

    /* Create thread that will continually broadcast connection info on multicast port for prospective clients. */
    /* make a copy of info to broadcast */
    bc_copy = (BC_INFO *) malloc(sizeof(BC_INFO));
    memcpy(bc_copy, &bc_info, sizeof(BC_INFO));
#ifdef __WIN32__
    thread = CreateThread(NULL, 0, tc_broadcast_conninfo, &bc_copy, 0, &threadId);
#else
    pthread_create(&thread, NULL, tc_broadcast_conninfo, bc_copy);
#endif

    /* Wait for other peers wanting to connect to me */
    i_am_client = 0;
    found_conn = 0;
    while (found_conn == 0) {

        do {
            nbytes = recvfrom(fd, &read_me, sizeof(SEND_ME), 0, NULL, NULL);
        } while ((nbytes == -1) && (tc_errno == TRICKCOMM_EINTR));

        if (nbytes == sizeof(read_me)) {

            /*
             * Read information from peer
             */
            read_me.port = ntohl((uint32_t) read_me.port);
            read_me.pid = ntohl((uint32_t) read_me.pid);
            read_me.conn_initiated = ntohl((uint32_t) read_me.conn_initiated);

            /* May I connect with this peer? Rules: Don't connect to myself! Don't connect to someone with same tag as
               myself Do connect to someone with same "other tag" */
            if (curr_pid != read_me.pid &&
                strcmp(bc_info.send_me.my_tag, read_me.my_tag) &&
                !strcmp(bc_info.send_me.other_tag, read_me.other_tag)) {

                /* Decide who will function as client Whoever has smaller pid will be the client If same pid, decide by
                   a string compare */
                if (curr_pid < read_me.pid) {
                    i_am_client = 1;
                } else if (curr_pid == read_me.pid) {
                    i_am_client = (strcmp(name, read_me.addr) > 0) ? 1 : 0;
                }

                /*
                 * Client Peer initiates the connection
                 */
                if (i_am_client == 1) {

                    /* Save off addr and port to connect */
                    size = strlen(read_me.addr) + 1;
                    dev_ptr->hostname = (char *) malloc((size_t) size);
                    strcpy(dev_ptr->hostname, read_me.addr);
                    dev_ptr->port = read_me.port;

                    read_me.conn_initiated = 1;
                    read_me.port = htonl((uint32_t) read_me.port);
                    read_me.pid = htonl((uint32_t) read_me.pid);
                    read_me.conn_initiated = htonl((uint32_t) read_me.conn_initiated);
                    sendto(bc_info.fd, (char *) &read_me, sizeof(SEND_ME), 0,
                           (struct sockaddr *) &bc_info.addr, (socklen_t) sizeof(bc_info.addr));
                    found_conn = 1;
                }

            } else if (curr_pid == read_me.pid &&
                       !strcmp(bc_info.send_me.addr, read_me.addr) &&
                       !strcmp(bc_info.send_me.my_tag, read_me.my_tag) &&
                       !strcmp(bc_info.send_me.other_tag, read_me.other_tag) && read_me.conn_initiated == 1) {

                found_conn = 1;
            }
        }
    }

    if (other_tag != (char *) NULL) {
        strncpy(dev_ptr->client_tag, other_tag, (size_t) TC_TAG_LENGTH);
        dev_ptr->client_tag[TC_TAG_LENGTH - 1] = '\0';
    }

    if (i_am_client == 1) {

        /* Client connects */

        /* Disconnect unneeded listen device */
        tc_disconnect(server_device);
        free((void *) server_device);

        tc_set_blockio_timeout_limit(dev_ptr, MAX_BLOCKIO_TIMEOUT_LIMIT);
        status = 1;
        while ((status != TC_SUCCESS) && (num_try < num_attempts)) {
            status = tc_connect(dev_ptr);
#if __WIN32__
            Sleep(200);
#else
            usleep(200000);
#endif
            num_try++;
        }
    } else {

        /*
         * Server accepts
         */
        status = 1;
        while (status != TC_SUCCESS && (num_try < num_attempts)) {
            if (tc_listen(server_device)) {
                status = tc_accept(server_device, dev_ptr);
                if (dev_ptr != NULL) {
                    tc_set_blockio_timeout_limit(dev_ptr, MAX_BLOCKIO_TIMEOUT_LIMIT);
                }
            }
            num_try++;
#if __WIN32__
            Sleep(200);
#else
            usleep(200000);
#endif
        }
        tc_disconnect(server_device);
        free((void *) server_device);
    }

    if (status != TC_SUCCESS) {
        trick_error_report(server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "could not open connection!\n");
        return (status);
    }

    /* Shutdown the broadcast */
    shutdown(bc_info.fd, 2);

    /* Return the pointer to the TCDevice as the tc_id. */
    return (TC_SUCCESS);
}
