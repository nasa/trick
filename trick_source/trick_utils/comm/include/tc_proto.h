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
     (Specify the trickcomm communications library API)
REFERENCE:
     ((None))
ASSUMPTIONS AND LIMITATIONS:
     ((None))
PROGRAMMERS:
    (((Kyle Harvey) (MDAC) (1993) (--) (Initial Version))
    ((Eddie J. Paddock) (LinCom) (March 1996) (--) (Modified Version))
    ((Scott Killingsworth) (LinCom) (July 1998) (--) (Cleanup and add WIN32)))
ICG:
     (No)
*/

/*
 *  $Id: tc_proto.h 2961 2013-05-06 21:35:42Z alin $
 */

#ifndef _tc_proto_h
#define _tc_proto_h

#include <stdarg.h>
#include "tc.h"

#ifdef STAND_ALONE
#include "attributes.h"
#else
#include "sim_services/MemoryManager/include/attributes.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize a communications server */
int tc_init(TCDevice * listen_device);
int tc_init_with_connection_info(TCDevice * listen_device, int domain, int type, const char * address, unsigned short in_port);

/* Check to see if a connection is pending */
int tc_listen(TCDevice * listen_device);

#define tc_accept(listen_device, device) tc_accept_(listen_device, device, __FILE__ , __LINE__ )
#define tc_connect(device) tc_connect_(device , __FILE__ , __LINE__ )

/* Accept a connection from a client */
int tc_accept_(TCDevice * listen_device, TCDevice * device , const char *file, int line);

/* Establish a connection to a server */
int tc_connect_(TCDevice * device, const char *file, int line);

/* Get system clock reference time */
double tc_clock_init(void);

/* Get elapsed time since last call to tc_clock_init() */
double tc_clock_time(double ref_time);

/* See if data is available for reading on a non-blocking connection */
int tc_pending(TCDevice * device);

/* Set a device to BLOCKIO or NOBLOCKIO - may be used as a toggle */
int tc_blockio(TCDevice * device, TCCommBlocking blockflag);

/* Check to see if a connection is valid */
int tc_isValid(TCDevice * device);

/* Set the time in secs that a blocking read 
 * will wait before timing out 
 */
int tc_set_blockio_timeout_limit(TCDevice * device, double limit);

#define tc_read(device, buffer, size)  tc_read_( device, buffer, size, __FILE__ , __LINE__ )
#define tc_write(device, buffer, size)  tc_write_( device, buffer, size, __FILE__ , __LINE__ )

/* Read data from a device */
int tc_read_(TCDevice * device,
             char *buffer, int size, const char *file, int line);

int tc_read_byteswap(TCDevice * device, char *buffer,
             int size, ATTRIBUTES * attr) ;

/* Write data to a device */
int tc_write_(TCDevice * device,
              char *buffer, int size, const char *file, int line);

/* Write data to a device and byte swap if remote and local
   byte orders are different */
int tc_write_byteswap(TCDevice * device,
                      char *buffer, int size, ATTRIBUTES * attr);

/* Disconnect a device */
int tc_disconnect(TCDevice * device);

/* Multiconnect */
int tc_multiconnect(TCDevice * dev_ptr,
                    char *my_tag,
                    char *other_tag, TrickErrorHndlr * error_handler);

/* */
int tc_init_udp_client( TCDevice *udp_client_device );

/* */
int tc_init_udp_server( TCDevice *udp_server_device );

/* */
int tc_init_mcast_client( TCDevice *mcast_client_device );

/* */
int tc_init_mcast_server( TCDevice *mcast_server_device );

#ifdef __WIN32__
DWORD WINAPI tc_broadcast_conninfo(LPVOID bc_info);
#else
void *tc_broadcast_conninfo(void *bc_info);
#endif

/* Turn On/Off Error reporting */
int tc_error(TCDevice * device, int on_off);

/* copy a device */
int tc_dev_copy(TCDevice * dest, TCDevice * src);

void *trick_bswap_buffer(void *out, void *in, ATTRIBUTES * attr, int tofrom) ;
#ifdef __cplusplus
}
#endif
#endif
