/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
LIBRARY DEPENDENCIES:
    (
    (WSSession.o)
    (WSSessionVariable.o)
    (http_GET_handlers.o)
    )
**************************************************************************/

/*
Messages sent from Client to Server
================================
{ "cmd" : "var_add",
  "var_name" : <str>
}
{ "cmd" : "var_pause" }
{ "cmd" : "var_unpause" }
{ "cmd" : "var_send" }
{ "cmd" : "var_clear" }
{ "cmd" : "var_exit" }
{ "cmd" : "var_cycle",
  "period" : <int>
}

Messages sent from Server to Client
=================================
{ "msg_type" : "error",
  "error_text" : <str>
}
{ "msg_type" : "var_list"
  "time" : <double>
  "values" : []
}
*/

#include "../include/http_server.h"
#include "../include/WSSession.hh"
#include "../include/http_GET_handlers.hh"

static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delete_method = MG_MK_STR("DELETE");
static const struct mg_str api_prefix = MG_MK_STR("/api/v1/");

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {

    http_message *hm = (struct http_message *)ev_data;
    HTTP_Server* hs = (HTTP_Server *)nc->user_data;

    switch(ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
            std::string uri(hm->uri.p, hm->uri.len);
            std::cout << "WEBSOCKET[" << (void*)nc << "] OPENED. URI=\"" << uri << "\"." << std::endl;
            // Create a session object to store information about this web-socket connection.
            WSsession* session = new WSsession(nc);
            hs->addSession(nc, session);
        } break;

        case MG_EV_WEBSOCKET_FRAME: { // Process websocket messages from the client (web browser).
            struct websocket_message *wm = (struct websocket_message *) ev_data;
            std::string msg ((char*)wm->data, wm->size);
            std::cout << "WEBSOCKET[" << (void*)nc << "] RECIEVED: " << msg << std::endl;
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                hs->handleClientMessage(nc, msg);
            }
        } break;
        case MG_EV_CLOSE: {
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                hs->deleteSession(nc);
                std::cout << "WEBSOCKET[" << (void*)nc << "] CLOSED." << std::endl;
            }
        } break;
        case MG_EV_POLL: {
           // The MG_EV_POLL event is sent to all connections for each invocation of mg_mgr_poll(),
           // called periodically by the threaded function connectionAttendant() [below].
           // Send websocket messages to the client (web browser).
           if (nc->flags & MG_F_IS_WEBSOCKET) {
               hs->sendSessionMessages(nc);
           }
        } break;
        case MG_EV_HTTP_REQUEST: {
            std::string uri(hm->uri.p, hm->uri.len);
            std::cout << "HTTP_REQUEST: URI = \"" << uri << "\"" << std::endl;
            if (mg_str_starts_with(hm->uri, api_prefix)) {
                if (mg_strcmp(hm->method, s_get_method)==0) {
                    std::string handlerName (hm->uri.p + api_prefix.len, hm->uri.len - api_prefix.len);
                    hs->handle_API_GET_request(nc, hm, handlerName);
                } else if (mg_strcmp(hm->method, s_put_method)==0) {
                    mg_http_send_error(nc, 405, "PUT method not allowed.");
                } else if (mg_strcmp(hm->method, s_delete_method)==0) {
                    mg_http_send_error(nc, 405, "DELETE method not allowed.");
                }
            } else {
               // Serve the files in the document-root directory, as specified by the URI.
               mg_serve_http(nc, (struct http_message *) ev_data, hs->http_server_options);
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

void HTTP_Server::install_API_GET_handler(std::string APIname, httpMethodHandler handler) {
    pthread_mutex_lock(&APIMapLock);
    httpMethodHandlerMap.insert(std::pair<std::string, httpMethodHandler>(APIname, handler));
    pthread_mutex_unlock(&APIMapLock);
}

void HTTP_Server::handle_API_GET_request(struct mg_connection *nc, http_message *hm, std::string handlerName) {
    std::map<std::string, httpMethodHandler>::iterator iter;
    iter = httpMethodHandlerMap.find(handlerName);
    if (iter != httpMethodHandlerMap.end()) {
        httpMethodHandler handler = iter->second;
        handler(nc, hm);
    } else {
       mg_http_send_error(nc, 404, "No such API.");
    }
}

void HTTP_Server::sendSessionMessages(struct mg_connection *nc) {
    // Find the session that goes with the given websocket connection,
    // and tell it to send its values to the client (web browser).
    std::map<mg_connection*, WSsession*>::iterator iter;
    iter = sessionMap.find(nc);
    if (iter != sessionMap.end()) {
        WSsession* session = iter->second;
        session->sendMessage();
    }
}

void HTTP_Server::deleteSession(struct mg_connection *nc) {
    std::map<mg_connection*, WSsession*>::iterator iter;
    iter = sessionMap.find(nc);
    if (iter != sessionMap.end()) {
        WSsession* session = iter->second;
        delete session;
        sessionMap.erase(iter);
    }
}
void HTTP_Server::handleClientMessage(struct mg_connection *nc, std::string msg) {
    std::map<mg_connection*, WSsession*>::iterator iter;
    iter = sessionMap.find(nc);
    if (iter != sessionMap.end()) {
        WSsession* session = iter->second;
        session->handleMessage(msg);
    }
}

void HTTP_Server::addSession(struct mg_connection *nc, WSsession* session) {
    pthread_mutex_lock(&sessionMapLock);
    sessionMap.insert( std::pair<mg_connection*, WSsession*>(nc, session) );
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

int HTTP_Server::http_init() {

    http_server_options.document_root = document_root;
    http_server_options.enable_directory_listing = "yes";

    install_API_GET_handler("vs_connections", &handle_HTTP_GET_vs_connections);
    install_API_GET_handler("alloc_info", &handle_HTTP_GET_alloc_info);

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
        std::map<mg_connection*, WSsession*>::iterator iter;
        pthread_mutex_lock(&sessionMapLock);
        for (iter = sessionMap.begin(); iter != sessionMap.end(); iter++ ) {
            WSsession* session = iter->second;
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
