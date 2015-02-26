/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
/*
PURPOSE:
     (Specify the trickcomm communications library API and structure definition)
PROGRAMMERS:
    (((Kyle Harvey) (MDAC) (1993) (--) (Initial Version))
    ((Eddie J. Paddock) (LinCom) (March 1996) (--) (Modified Version))
    ((Scott Killingsworth) (LinCom) (July 1998) (--) (Cleanup and add WIN32)))
*/

/*
 *  $Id: tc.h 2977 2013-05-13 20:20:08Z alin $
 */

#ifndef _tc_h
#define _tc_h

#ifdef __WIN32__
#   include <winsock2.h>
#   include <ws2tcpip.h>
#endif

#include <stdio.h>

#ifndef __WIN32__
#  include <errno.h>
#  include <unistd.h>
#  include <pthread.h>
#  include <signal.h>
#  include <netdb.h>
#  if (_POSIX_C_SOURCE)
#     ifndef TCP_NODELAY
#        define TCP_NODELAY   0x01
#     endif
#  else
#     include <netinet/tcp.h>
#  endif
#  if ( __vxworks )
#     include <sys/times.h>
#     include <ioLib.h>
#  else
#     if ( __Lynx__ ) 
#         include <socket.h>
#         include <time.h>
#         define __NO_INCLUDE_WARN__
#         include <netinet/in.h>
#         undef  __NO_INCLUDE_WARN__
#     else
#         include <sys/socket.h>
#         include <sys/time.h>
#         include <netinet/in.h>
#         include <arpa/inet.h>
#     endif
#     include <sys/ioctl.h>
#     include <sys/utsname.h>
#  endif
#  if ( __sun )
#     include <sys/filio.h>
#  endif
#  if ( __CYGWIN__ )
#     include <sys/param.h>
#  endif
#endif

#include "trick_byteswap.h"
#include "trick_error_hndlr.h"

#define TC_DEFAULT_PORT 5132

#if (__linux)
#  define TC_NOSIGNAL MSG_NOSIGNAL
#else
#  define TC_NOSIGNAL 0
#endif

/* Status codes */
#define TC_SUCCESS                      0
#define TC_COULD_NOT_OPEN_SOCKET        1
#define TC_COULD_NOT_BIND_SOCKET        2
#define TC_COULD_NOT_LISTEN_SOCKET      3
#define TC_COULD_NOT_ACCEPT             4
#define TC_DRIVER_NOT_INSTALLED         5
#define TC_DRIVER_ALREADY_INSTALLED     6
#define TC_DRIVER_IS_NULL               7
#define TC_HOST_NOT_FOUND               8
#define TC_COULD_NOT_CONNECT            9
#define TC_RESOURCE_EXCEEDED            10
#define TC_DRIVER_NOT_FOUND             11
#define TC_DRIVER_ALREADY_DISCONNECTED  12
#define TC_CONN_DISABLED                13

#define TC_COULD_NOT_SET_TTL            14
#define TC_COULD_NOT_ADD_MEMBERSHIP     15

#define TC_EWOULDBLOCK                  -1
#define TC_READWRITE_TIMEOUT            -2
#define TC_BROKEN_PIPE                  -3

/* Set default for blocking timeout limit high */
#define MAX_BLOCKIO_TIMEOUT_LIMIT   1.0e20

/* WIN32 Support */
#ifdef __WIN32__
#  define TRICKCOMM_SOCKET_FAMILY PF_INET
#  define TRICKCOMM_SOCKET_PROTO IPPROTO_TCP
#  define TRICKCOMM_INVALID_SOCKET -1
#  define TRICKCOMM_EWOULDBLOCK WSAEWOULDBLOCK
#  define TRICKCOMM_EINTR WSAEINTR
#  define TRICKCOMM_EAGAIN WSATRY_AGAIN
#  define TRICKCOMM_EPIPE WSAENOTCONN
#  define EWOULDBLOCK WSAEWOULDBLOCK
#  define CLOSE_SOCKET closesocket
#  define IOCTL_SOCKET ioctlsocket
#  define tc_errno WSAGetLastError()
int tc_StartWinSock(void);
#else
#  define TRICKCOMM_SOCKET_FAMILY AF_INET
#  define TRICKCOMM_SOCKET_PROTO 0
#  define TRICKCOMM_INVALID_SOCKET -1
#  define TRICKCOMM_EWOULDBLOCK EWOULDBLOCK
#  define TRICKCOMM_EINTR EINTR
#  define TRICKCOMM_EAGAIN EAGAIN
#  define TRICKCOMM_EPIPE EPIPE
#  define CLOSE_SOCKET close
#  define IOCTL_SOCKET ioctl
#  define tc_errno errno
#endif

/* Define a macro to yield the processor - TC_RELEASE() */
#if ( __sgi )
#  define TC_RELEASE() sginap(0)
typedef int socket_t ;
#elif ( __linux | __CYGWIN__ | __vxworks | __APPLE__ | __Lynx__ | __QNX__ | __ghs | __INTERIX )
#  define TC_RELEASE() usleep(1)
#elif ( __sun )
#  define TC_RELEASE() { struct timeval timeout;\
                    timeout.tv_sec = 0;\
                    timeout.tv_usec = 0;\
                    select(0,0,0,0,&timeout);}
#elif ( __WIN32__ )
#  define TC_RELEASE() Sleep(0)
#endif

#define TC_TAG_LENGTH 80
#define TC_BYTE_ORDER_NDX   0
#define TC_LONG_SIZE_NDX    1
#define TC_BYTE_INFO_LENGTH 2
#define TC_MULT_PORT 12345
#define TC_MULT_GROUP "225.0.0.40"

#ifdef STAND_ALONE
#define send_hs fprintf
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TC_COMM_FALSE = 0,      /* -- False flag value    */
    TC_COMM_TRUE = 1,       /* -- True flag value     */
    TC_COMM_NO = 0,         /* -- No flag value       */
    TC_COMM_YES = 1,        /* -- Yes flag value      */
    TC_COMM_DISABLED = 0,   /* -- Disabled flag value */
    TC_COMM_ENABLED = 1     /* -- Enabled flag value  */
} TCCommFlag;

/* 
 * WARNING : These flags are the reverse logic of BLOCKIO and NOBLOCKIO.
 *           This is so that if a user does not set the blockio_flag
 *          in a device, the device will default to blocking
 */
typedef enum {
    TC_COMM_BLOCKIO = 0,        /* -- Set blocking I/O     */
    TC_COMM_NOBLOCKIO = 1,      /* -- Set non-blocking I/O */
    TC_COMM_TIMED_BLOCKIO = 2,  /* -- Set timed software block_io */
    TC_COMM_ALL_OR_NOTHING = 3  /* -- Set non-blocking to 
                                          all or nothing */
} TCCommBlocking;

typedef struct {

    int socket;                        /* ** Socket file descriptor */
    int socket_type ;                  /* ** Socket type SOCK_STREAM or SOCK_DGRAM */
    TCCommFlag disabled;               /* -- Connection enabled flag */
    TCCommFlag disable_handshaking;    /* -- Set this to skip handshaking in tc_connect and tc_accept. */
    int dmtcp_use_real ;               /* -- if true, use dlsym to lookup connect/accept calls. Used with DMTCP sockets
                                             if false call connect/accept directly. */
    double blockio_limit;              /* s  Block I/O timeout limit */
    TCCommBlocking blockio_type;       /* -- E.g. spinloops */
    unsigned char byte_info[TC_BYTE_INFO_LENGTH]; /* ** byte swap info */
    int client_id;                     /* -- client id, other side.         Set by tc_accept. */
    char client_tag[TC_TAG_LENGTH];    /* -- client tag, other side.        User specified.
                                                                              Defaults if unspecified.*/
    struct in_addr client_addr;        /* ** client tag, other side.        Set by tc_connect. */
    TrickErrorHndlr *error_handler;    /* ** error handler.                 User specified.
                                                                              Defaults if unspecified.*/
    char *hostname;                    /* -- Hostname of server/client.     User specified. */
    int port;                          /* -- TCP/IP Port number.            User specified. */
    struct sockaddr_in remoteServAddr; /* ** Remote server address for UDP. Set by tc_init_udp_client or
                                                                                   tc_init_mcast_client */
    char *mcast_group;                 /* -- Multicast group address.       User specified. */
    unsigned char ttl ;                /* ** IP MULTICAST "Time to live"    User specified. */
    struct sockaddr_in cliAddr;        /* ** Client Address from tc_read for UDP. Set by tc_read. */
} TCDevice;

/* Struct to send over the multicast port */
typedef struct {
    char addr[40];                  /* ** address of connection */
    int port;                       /* ** port of connection */
    int pid;                        /* ** pid of connection */
    int conn_initiated;             /* ** Return flag to initiate 
                                          connection */
    int disabled;                   /* ** 0=Communication enabled
                                          1=device disabled */
    char my_tag[TC_TAG_LENGTH];     /* ** tag of my process */
    char other_tag[TC_TAG_LENGTH];  /* ** tag of the other side */
} SEND_ME;

/* Multicast broadcasting info struct */
typedef struct {
    SEND_ME send_me;            /* ** Struct to continuously send */
    TCDevice *device;           /* ** Device to connect to */
    struct sockaddr_in addr;    /* ** Address to multicast on */
    int fd;                     /* ** File descriptor to write to */
} BC_INFO;

#ifdef __cplusplus
}
#endif

#endif
