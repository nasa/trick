/*************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server.)
LIBRARY DEPENDENCIES:
    ( (../src/http_server.cpp))
**************************************************************************/
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <string>
#include <map>
#include <mongoose.h>
#include <pthread.h>
#include "../include/WebSocketSession.hh"

typedef void (*httpMethodHandler)(struct mg_connection *, struct http_message *);
typedef WebSocketSession* (*WebSocketSessionMaker)(struct mg_connection *nc);

class HTTP_Server {
    public:
        const char* port;
        const char* document_root;
        struct mg_mgr mgr;               /* ** mongoose */
        struct mg_connection *listener;  /* ** mongoose */
        pthread_t server_thread;         /* ** */
        bool shutting_down;

        std::map< std::string, httpMethodHandler> httpGETHandlerMap;     /* ** */
        pthread_mutex_t httpGETHandlerMapLock;                           /* ** */
        std::map< std::string, WebSocketSessionMaker> WebSocketSessionMakerMap; /* ** */
        pthread_mutex_t WebSocketSessionMakerMapLock;                           /* ** */
        std::map<mg_connection*, WebSocketSession*> sessionMap;          /* ** */
        pthread_mutex_t sessionMapLock;                                  /* ** */
        pthread_mutex_t serviceLock;                                     /* ** */
        struct mg_serve_http_opts http_server_options;                   /* ** mongoose*/
        struct mg_bind_opts bind_opts;                                   /* ** mongoose*/
        pthread_cond_t serviceConnections;                               /* ** */

        // Trick Job-functions
        int http_default_data();
        int http_init();
        int http_top_of_frame();
        int http_shutdown();

        void installWebSocketSessionMaker(std::string name, WebSocketSessionMaker creater);
        void installHTTPGEThandler(std::string APIname, httpMethodHandler handler);

        // These are internals and should be considered public. They are not private only
        // because they need to be callable from the servers event handler.
        void sendWebSocketSessionMessages(struct mg_connection *nc);
        void handleWebSocketClientMessage(struct mg_connection *nc, std::string msg);
        void addWebSocketSession(struct mg_connection *nc, WebSocketSession* session);
        void deleteWebSocketSession(struct mg_connection *nc);
        WebSocketSession* makeWebSocketSession(struct mg_connection *nc, std::string name);
        void handleHTTPGETrequest(struct mg_connection *nc, http_message *hm, std::string handlerName);
};
#endif
