/*************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
**************************************************************************/
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <mongoose.h>
#include <pthread.h>

typedef struct {

    struct mg_mgr mgr;         /* ** mongoose */
    struct mg_connection *nc;  /* ** mongoose */
    const char* port;
    pthread_t server_thread;   /* ** */
    bool shutting_down;

} HTTP_Server ;

#ifdef __cplusplus
extern "C" {
#endif
    int http_default_data(HTTP_Server * S) ;
    int http_init(HTTP_Server * S) ;
    int http_shutdown(HTTP_Server * S) ;
#ifdef __cplusplus
}
#endif

#endif
