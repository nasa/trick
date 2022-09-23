/*
PURPOSE:
     (Allows clients to get and set Trick parameters)
PROGRAMMERS:
    (((Keith Vetter) (LinCom) (September 2001) (--)))
*/

#ifndef VARIABLE_SERVER_H
#define VARIABLE_SERVER_H

#include <pthread.h>

#include "trick/reference.h"
#include "trick/tc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    REF2 ref;
    double scale_factor;
    double bias;
} VARIABLE_NODE;

typedef struct {
    int socket ;              /* ** multicast socket file descriptor */
    struct sockaddr_in addr ; /* ** multicast socket info */
} MCAST_INFO;

typedef struct VARIABLE_SERVER_struct {
    int enabled ;
    int debug;          /* ** Variable_server debugging */
    int synchronous ;
    double update_rate;
    double next;
    int binary_data;
    int no_handshake;
    int num_vars;
    int clear_cmd;
    int send_cmd;
    int pause_cmd;
    int exists_cmd;
    int exit_cmd;
    VARIABLE_NODE *vars;

    int multicast_on ;   /* -- flag to switch between TCdevice and multcast socket */
    TCDevice * connection ;
    MCAST_INFO mcast_info ;
    char local_byteorder ;

    pthread_t thread_id ;
} VARIABLE_SERVER;

//void *var_server(void *);       /* Server thread */
//void *var_serve_init(void *);   /* Initialize server */
//void exit_var_server(void *);   /* Exit handler for var init server */
//void exit_var_serv_threads( void );  /* Exit handler for var init server */

int vs_mcast_init(MCAST_INFO * M, char * address , int port ) ;
//int vs_write_data(VARIABLE_SERVER * V) ;
//int vs_write_error(VARIABLE_SERVER * V, char * incoming_str , char * error_str ) ;
//int vs_format_ascii(REF2 * ref, double scale_factor, double bias, char *value) ;

#ifdef __cplusplus
}
#endif
#endif
