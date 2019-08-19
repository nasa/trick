/*************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
LIBRARY DEPENDENCIES:
    ( (../src/http_server.cpp))
**************************************************************************/
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <mongoose.h>
#include <pthread.h>
#include <string>
#include <map>
#include "../include/WebSocketSession.hh"

typedef void (*httpMethodHandler)(struct mg_connection *, struct http_message *);

class HTTP_Server {
    public:
        const char* port;
        const char* document_root;
        struct mg_mgr mgr;               /* ** mongoose */
        struct mg_connection *listener;  /* ** mongoose */
        pthread_t server_thread;         /* ** */
        bool shutting_down;

        std::map< std::string, httpMethodHandler> httpMethodHandlerMap;  /* ** */
        pthread_mutex_t APIMapLock;                                      /* ** */
        std::map<mg_connection*, WebSocketSession*> sessionMap;                 /* ** */
        pthread_mutex_t sessionMapLock;                                  /* ** */
        struct mg_serve_http_opts http_server_options;                   /* ** mongoose*/
        struct mg_bind_opts bind_opts;                                   /* ** mongoose*/
        pthread_cond_t serviceConnections;                               /* ** */

        // Trick Job-functions
        int http_default_data();
        int http_init();
        int http_top_of_frame();
        int http_shutdown();

        void sendSessionMessages(struct mg_connection *nc);
        void handleClientMessage(struct mg_connection *nc, std::string msg);
        void addSession(struct mg_connection *nc, WebSocketSession* session);
        void deleteSession(struct mg_connection *nc);
        void install_API_GET_handler(std::string APIname, httpMethodHandler handler);
        void handle_API_GET_request(struct mg_connection *nc, http_message *hm, std::string handlerName);
};
#endif
