/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
LIBRARY DEPENDENCIES:
    ((VariableServerSession.o)
    (http_GET_handlers.o))
**************************************************************************/
#include <iostream>
#include "../include/http_server.hh"
#include "../include/http_GET_handlers.hh"
#include "../include/VariableServerSession.hh"

static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delete_method = MG_MK_STR("DELETE");
static const struct mg_str http_api_prefix = MG_MK_STR("/api/http/");
static const struct mg_str ws_api_prefix = MG_MK_STR("/api/ws/");
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {

    http_message *hm = (struct http_message *)ev_data;
    HTTP_Server* httpServer = (HTTP_Server *)nc->user_data;

    switch(ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: { // Process new websocket connection.
            std::string uri(hm->uri.p, hm->uri.len);
            std::cout << "WEBSOCKET_REQUEST: URI = \"" << uri << "\"" << std::endl;
            if (mg_str_starts_with(hm->uri, ws_api_prefix)) {
                std::string wsType (hm->uri.p + ws_api_prefix.len, hm->uri.len - ws_api_prefix.len);
                WebSocketSession* session = httpServer->makeWebSocketSession(nc, wsType);
                if (session != NULL) {
                    httpServer->addWebSocketSession(nc, session);
                    std::cout << "WEBSOCKET[" << (void*)nc << "] OPENED. URI=\"" << uri << "\"." << std::endl;
                } else {
                    nc->flags |= MG_F_SEND_AND_CLOSE;
                    std::cout << "ERROR: No such web socket interface: \"" << uri << "\"." << std::endl;
                }
            }
        } break;
        case MG_EV_WEBSOCKET_FRAME: { // Process websocket messages from the client (web browser).
            struct websocket_message *wm = (struct websocket_message *) ev_data;
            std::string msg ((char*)wm->data, wm->size);
            std::cout << "WEBSOCKET[" << (void*)nc << "] RECIEVED: " << msg << std::endl;
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                httpServer->handleWebSocketClientMessage(nc, msg);
            }
        } break;
        case MG_EV_CLOSE: { // Process closed websocket connection.
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                httpServer->deleteWebSocketSession(nc);
                std::cout << "WEBSOCKET[" << (void*)nc << "] CLOSED." << std::endl;
            }
        } break;
        case MG_EV_POLL: {
           // The MG_EV_POLL event is sent to all connections for each invocation of mg_mgr_poll(),
           // called periodically by the threaded function connectionAttendant() [below].
           // Send websocket messages to the client (web browser).
           if (nc->flags & MG_F_IS_WEBSOCKET) {
               httpServer->sendWebSocketSessionMessages(nc);
           }
        } break;
        case MG_EV_HTTP_REQUEST: { // Process HTTP requests.
            std::string uri(hm->uri.p, hm->uri.len);
            std::cout << "HTTP_REQUEST: URI = \"" << uri << "\"" << std::endl;
            if (mg_str_starts_with(hm->uri, http_api_prefix)) {
                if (mg_strcmp(hm->method, s_get_method)==0) {
                    std::string handlerName (hm->uri.p + http_api_prefix.len, hm->uri.len - http_api_prefix.len);
                    httpServer->handleHTTPGETrequest(nc, hm, handlerName);
                } else if (mg_strcmp(hm->method, s_put_method)==0) {
                    mg_http_send_error(nc, 405, "PUT method not allowed.");
                } else if (mg_strcmp(hm->method, s_delete_method)==0) {
                    mg_http_send_error(nc, 405, "DELETE method not allowed.");
                }
            } else {
               // Serve the files in the document-root directory, as specified by the URI.
               mg_serve_http(nc, (struct http_message *) ev_data, httpServer->http_server_options);
            }
        } break;
        default: {
        } break;
    }
}

// =========================================================================
// This function runs in its own pthread to operate the webserver.
// =========================================================================
static void* connectionAttendant (void* arg) {
   HTTP_Server *S = (HTTP_Server*)arg;
   while(1) {
        pthread_mutex_lock(&S->sessionMapLock);
        // Wait here until the serviceConnections condition is signaled by the top_of_frame job.
        pthread_cond_wait(&S->serviceConnections, &S->sessionMapLock);
        if (S->shutting_down) {
            pthread_mutex_unlock(&S->sessionMapLock);
            return NULL;
        } else {
            mg_mgr_poll(&S->mgr, 50);
        }
        pthread_mutex_unlock(&S->sessionMapLock);
    }
    return NULL;
}

void HTTP_Server::installWebSocketSessionMaker(std::string name, WebSocketSessionMaker maker) {
    pthread_mutex_lock(&WebSocketSessionMakerMapLock);
    WebSocketSessionMakerMap.insert(std::pair<std::string, WebSocketSessionMaker>(name, maker));
    pthread_mutex_unlock(&WebSocketSessionMakerMapLock);
}

WebSocketSession* HTTP_Server::makeWebSocketSession(struct mg_connection *nc, std::string name) {
    std::map<std::string, WebSocketSessionMaker>::iterator iter;
    iter = WebSocketSessionMakerMap.find(name);
    if (iter != WebSocketSessionMakerMap.end()) {
        WebSocketSessionMaker maker = iter->second;
        return maker(nc);
    } else {
        return NULL;
       mg_http_send_error(nc, 404, "No such API.");
    }
}

void HTTP_Server::installHTTPGEThandler(std::string APIname, httpMethodHandler handler) {
    pthread_mutex_lock(&httpGETHandlerMapLock);
    httpGETHandlerMap.insert(std::pair<std::string, httpMethodHandler>(APIname, handler));
    pthread_mutex_unlock(&httpGETHandlerMapLock);
}

void HTTP_Server::handleHTTPGETrequest(struct mg_connection *nc, http_message *hm, std::string handlerName) {
    std::map<std::string, httpMethodHandler>::iterator iter;
    iter = httpGETHandlerMap.find(handlerName);
    if (iter != httpGETHandlerMap.end()) {
        httpMethodHandler handler = iter->second;
        handler(nc, hm);
    } else {
       mg_http_send_error(nc, 404, "No such API.");
    }
}

void HTTP_Server::sendWebSocketSessionMessages(struct mg_connection *nc) {
    // Find the session that goes with the given websocket connection,
    // and tell it to send its values to the client (web browser).
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    iter = sessionMap.find(nc);
    if (iter != sessionMap.end()) {
        WebSocketSession* session = iter->second;
        session->sendMessage();
    }
}

void HTTP_Server::deleteWebSocketSession(struct mg_connection *nc) {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    iter = sessionMap.find(nc);
    if (iter != sessionMap.end()) {
        WebSocketSession* session = iter->second;
        delete session;
        sessionMap.erase(iter);
    }
}
void HTTP_Server::handleWebSocketClientMessage(struct mg_connection *nc, std::string msg) {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    iter = sessionMap.find(nc);
    if (iter != sessionMap.end()) {
        WebSocketSession* session = iter->second;
        session->handleMessage(msg);
    }
}

void HTTP_Server::addWebSocketSession(struct mg_connection *nc, WebSocketSession* session) {
    pthread_mutex_lock(&sessionMapLock);
    sessionMap.insert( std::pair<mg_connection*, WebSocketSession*>(nc, session) );
    pthread_mutex_unlock(&sessionMapLock);
}

// =========================================================================
// Trick Sim Interface Functions
// =========================================================================

int HTTP_Server::http_default_data() {
    port = "8888";
    document_root = "www";
    shutting_down = false;
    return 0;
}

static WebSocketSession* makeVariableServerSession( struct mg_connection *nc ) {
    return new VariableServerSession(nc);
}

int HTTP_Server::http_init() {

    http_server_options.document_root = document_root;
    http_server_options.enable_directory_listing = "yes";

    installHTTPGEThandler("vs_connections", &handle_HTTP_GET_vs_connections);
    installHTTPGEThandler("alloc_info", &handle_HTTP_GET_alloc_info);
    installWebSocketSessionMaker("VariableServer", &makeVariableServerSession);
    //installWebSocketSessionMaker("VariableServer", [](struct mg_connection *nc) -> WebSocketSession* { return new VariableServerSession(nc); } );

    mg_mgr_init( &mgr, NULL );

    memset(&bind_opts, 0, sizeof(bind_opts));
    bind_opts.user_data = this;
    listener = mg_bind_opt( &mgr, port, ev_handler, bind_opts);

    if (listener != NULL) {
        std::cout << "Trick Webserver: Starting, and listening on port " << port << ".\n"
                  << "Trick Webserver: Document root = \"" << document_root << "\""
                  << std::endl;
    } else {
        std::cerr << "Trick Webserver: ERROR: Failed to create listener.\n"
                  << "Perhaps another program is already using port " << port << "."
                  << std::endl;
        return 1;
    }
    mg_set_protocol_http_websocket( listener );
    pthread_cond_init(&serviceConnections, NULL);
    pthread_create( &server_thread, NULL, connectionAttendant, (void*)this );
    return 0;
}

int HTTP_Server::http_top_of_frame() {
    if (listener != NULL) {
        // Have all of the sessions stage their data. We do this here, in a
        // top_of_frame job, so that all of the data is time-homogeneous.
        std::map<mg_connection*, WebSocketSession*>::iterator iter;
        pthread_mutex_lock(&sessionMapLock);
        for (iter = sessionMap.begin(); iter != sessionMap.end(); iter++ ) {
            WebSocketSession* session = iter->second;
            session->stageData();
        }
        pthread_mutex_unlock(&sessionMapLock);
        // Signal the server thread to construct and send the values-message to the client.
        pthread_cond_signal( &serviceConnections );
    }
    return 0;
}

int HTTP_Server::http_shutdown() {
    if (listener != NULL) {
        std::cout << "Trick Webserver: Shutting down on port " << port << "." << std::endl;
        shutting_down = true;

        // Send the serviceConnections signal one last time so the connectionAttendant thread can quit.
        pthread_cond_signal( &serviceConnections );
        pthread_join(server_thread, NULL);
    }
    return 0;
}
