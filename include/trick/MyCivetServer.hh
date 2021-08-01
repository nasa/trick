/*************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server.)
**************************************************************************/
#ifndef CIVET_SERVER_H
#define CIVET_SERVER_H

#ifdef USE_CIVET

#include <string>
#include <map>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include "trick/WebSocketSession.hh"

typedef WebSocketSession* (*WebSocketSessionMaker)(struct mg_connection *nc);
typedef void (*httpMethodHandler)(struct mg_connection *, void* cbdata);

class MyCivetServer {
    public:

        unsigned int port; 
        const char* document_root;
        bool enable;       
        bool debug;        
        bool time_homogeneous;
        const char* path_to_ssl_cert;
        bool ssl_enable;
        const char* error_log_file;

        struct mg_context *ctx; /* ** civetweb */

        // Trick Job-Functins
        int default_data();
        int shutdown();
        int init();
        int join();
        int http_top_of_frame();

        //TODO: Make these private and fix threading design issue
        pthread_t server_thread;                                                /* ** */
        bool sessionDataMarshalled;                                             /* ** */
        pthread_mutex_t lock_loop;                                              /* ** */
	    pthread_cond_t cond_loop;                                               /* ** */
	    bool service_connections = true;                                        /* ** */
        bool shutting_down;                                                     /* ** */

        std::map<std::string, WebSocketSessionMaker> WebSocketSessionMakerMap;  /* ** */
        pthread_mutex_t WebSocketSessionMakerMapLock;                           /* ** */

        std::map<mg_connection*, WebSocketSession*> webSocketSessionMap;        /* ** */
        pthread_mutex_t WebSocketSessionMapLock;                                /* ** */

        std::map< std::string, httpMethodHandler> httpGETHandlerMap;            /* ** */
        pthread_mutex_t httpGETHandlerMapLock;                                  /* ** */

        void addWebSocketSession(struct mg_connection *nc, WebSocketSession* session);
        WebSocketSession* makeWebSocketSession(struct mg_connection *nc, std::string name);
        void marshallWebSocketSessionData();
        void sendWebSocketSessionMessages(struct mg_connection *nc);
        void unlockConnections();
        void deleteWebSocketSession(struct mg_connection * nc);
        void installHTTPGEThandler(std::string handlerName, httpMethodHandler handler);
        void installWebSocketSessionMaker(std::string name, WebSocketSessionMaker maker);
        void handleWebSocketClientMessage(struct mg_connection *conn, const char* data);
        void handleHTTPGETrequest(struct mg_connection *conn, const struct mg_request_info* ri, std::string handlerName);


        std::string tmp_string;



        // void installWebSocketSessionMaker(std::string name, WebSocketSessionMaker maker);

        
};

struct Data {
    MyCivetServer* server;
    std::string name;
};

#endif
#endif
