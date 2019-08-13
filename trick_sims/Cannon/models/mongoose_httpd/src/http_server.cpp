/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
LIBRARY DEPENDENCIES:
    (
    (WSSession.o)
    (WSSessionVariable.o)
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

#include <sstream>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "../include/http_server.h"
#include "../include/WSSession.hh"
#include "trick/exec_proto.h"

#include "trick/VariableServer.hh"
extern Trick::VariableServer * the_vs ;
#include "trick/MemoryManager.hh"
extern Trick::MemoryManager* trick_MM;

static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delete_method = MG_MK_STR("DELETE");
static const struct mg_str api_prefix = MG_MK_STR("/api/v1");

// ============================================================================
// HTTP GET Handlers
// ============================================================================

// Respond to HTTP GET method with URI="/api/v1/vs_connections".
void handle_HTTP_GET_vs_connections(struct mg_connection *nc, struct http_message *hm) {
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    ss << *the_vs << std::endl;
    std::string tmp = ss.str();
    mg_printf_http_chunk(nc, "%s", tmp.c_str());
    mg_send_http_chunk(nc, "", 0);
}

int getIntegerQueryValue(struct http_message *hm, const char* key, int defaultVal) {
    char value_text[100];
    if ( mg_get_http_var(&(hm->query_string), key, value_text, sizeof(value_text)) > 0) {
        return atoi(value_text);
    } else {
        return defaultVal;
    }
}

// Respond to HTTP GET method with URI="/api/v1/alloc_info".
void handle_HTTP_GET_alloc_info(struct mg_connection *nc, struct http_message *hm) {
    int start = getIntegerQueryValue(hm, "start", 0);
    int count = getIntegerQueryValue(hm, "count", 10);
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    trick_MM->write_JSON_alloc_list(ss, start, count);
    std::string tmp = ss.str();
    mg_printf_http_chunk(nc, "%s", tmp.c_str());
    mg_send_http_chunk(nc, "", 0);
}

#define DEBUG
static struct mg_serve_http_opts http_server_options;
std::map<mg_connection*, WSsession*> sessionMap;
pthread_mutex_t sessionMapLock;
pthread_cond_t serviceConnections;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {

    http_message *hm = (struct http_message *)ev_data;

    switch(ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {

#ifdef DEBUG
            char * s = strndup(hm->uri.p, hm->uri.len);
            printf("WEBSOCKET[%p] OPENED. URI=\"%s\"\n", nc, s);
            free(s);
#endif
            // Create a session object to store information about this web-socket connection.
            WSsession* session = new WSsession(nc);

            pthread_mutex_lock(&sessionMapLock);
            sessionMap.insert( std::pair<mg_connection*, WSsession*>(nc, session) );
            pthread_mutex_unlock(&sessionMapLock);

        } break;

        case MG_EV_WEBSOCKET_FRAME: {
            // --------------------------------------------------------
            // Process websocket messages from the client (web browser).
            // --------------------------------------------------------
            struct websocket_message *wm = (struct websocket_message *) ev_data;
            char* msg = strndup((char*)wm->data, wm->size);

#ifdef DEBUG
            printf("WEBSOCKET[%p] RECIEVED: %s\n", nc, msg);
#endif
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                // Find the session that goes with this connection.
                std::map<mg_connection*, WSsession*>::iterator iter;
                iter = sessionMap.find(nc);
                if (iter != sessionMap.end()) {
                    WSsession* session = iter->second;
                    session->handle_msg(msg);
                }
            }
            free(msg);
        } break;
        case MG_EV_CLOSE: {
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                std::map<mg_connection*, WSsession*>::iterator iter;
                iter = sessionMap.find(nc);
                if (iter != sessionMap.end()) {
                    WSsession* session = iter->second;
                    delete session;
                    sessionMap.erase(iter);
                }
#ifdef DEBUG
                printf("WEBSOCKET[%p] CLOSED.\n", nc);
#endif
            }
        } break;
        case MG_EV_POLL: {
           // The MG_EV_POLL event is sent to all connections for each invocation of mg_mgr_poll().
           // The threaded function connectionAttendant() [below] periodically calls mg_mgr_poll().

           // Send websocket messages to the client (web browser).
           if (nc->flags & MG_F_IS_WEBSOCKET) {

               // Find the session that goes with the given websocket connection,
               // and tell it to send its values to the client (web browser).
               std::map<mg_connection*, WSsession*>::iterator iter;
               iter = sessionMap.find(nc);
               if (iter != sessionMap.end()) {
                   WSsession* session = iter->second;
                   session->sendValues();
               }
           }
        } break;
        case MG_EV_HTTP_REQUEST: {

#ifdef DEBUG
            char * s = strndup(hm->uri.p, hm->uri.len);
            printf("HTTP_REQUEST: URI = \"%s\"\n", s);
            free(s);
#endif
            if (mg_str_starts_with(hm->uri, api_prefix)) {
                struct mg_str key;
                key.p = hm->uri.p + api_prefix.len;
                key.len = hm->uri.len - api_prefix.len;

                if (mg_strcmp(hm->method, s_get_method)==0) {
                    if (mg_vcmp(&key, "/vs_connections") == 0) {
                        handle_HTTP_GET_vs_connections(nc, hm);
                    } else if (mg_vcmp(&key, "/alloc_info") == 0) {
                        handle_HTTP_GET_alloc_info(nc, hm);
                    } else {
                        mg_http_send_error(nc, 404, "No such API.");
                    }
                } else if (mg_strcmp(hm->method, s_put_method)==0) {
                    mg_http_send_error(nc, 405, "PUT method not allowed.");
                } else if (mg_strcmp(hm->method, s_delete_method)==0) {
                    mg_http_send_error(nc, 405, "DELETE method not allowed.");
                }
            } else {
               // Serve the files in the document-root directory, as specified by the URI.
               mg_serve_http(nc, (struct http_message *) ev_data, http_server_options);
            }
        } break;
        default: {
        } break;
    }
}

// =========================================================================
// This function runs in its own pthread to operate the webserver.
// =========================================================================
void* connectionAttendant (void* arg) {
   HTTP_Server *S = (HTTP_Server*)arg;
   while(1) {
        pthread_mutex_lock(&sessionMapLock);
        // Wait here until the serviceConnections condition is signaled by the top_of_frame job.
        pthread_cond_wait(&serviceConnections, &sessionMapLock);
        if (S->shutting_down) {
            pthread_mutex_unlock(&sessionMapLock);
            return NULL;
        } else {
            mg_mgr_poll(&S->mgr, 50);
        }
        pthread_mutex_unlock(&sessionMapLock);
    }
    return NULL;
}

// =========================================================================
// Trick Sim Interface Functions
// =========================================================================

int http_default_data(HTTP_Server *S) {
    S->port = "8888";
    S->shutting_down = false;
    S->document_root = "www";
    return 0;
}

int http_init(HTTP_Server *S) {

    http_server_options.document_root = S->document_root;
    http_server_options.enable_directory_listing = "yes";

    mg_mgr_init(&S->mgr, NULL);

    std::cout << "Trick Webserver: Starting, and listening on port " << S->port << ".\n"
              << "Trick Webserver: Document root = \"" << S->document_root << "\""
              << std::endl;

    S->nc = mg_bind(&S->mgr, S->port, ev_handler);
    if (S->nc == NULL) {
      std::cerr << "Trick Webserver: ERROR: Failed to create listener.\n"
                << "Perhaps another program is already using port " << S->port << "."
                << std::endl;
      return 1;
    }
    mg_set_protocol_http_websocket(S->nc);

    pthread_cond_init(&serviceConnections, NULL);
    pthread_create( &S->server_thread, NULL, connectionAttendant, (void*)S);
    return 0;
}

int http_top_of_frame(HTTP_Server * S) {
    if (S->nc != NULL) {

        // Have all of the sessions stage their data. We do this here, in a
        // top_of_frame job, so that all of the data is time-homogeneous.
        std::map<mg_connection*, WSsession*>::iterator iter;
        pthread_mutex_lock(&sessionMapLock);
        for (iter = sessionMap.begin(); iter != sessionMap.end(); iter++ ) {
            WSsession* session = iter->second;
            session->stageValuesSynchronously();
        }
        pthread_mutex_unlock(&sessionMapLock);

        // Signal the server thread to construct and send the values-message to the client.
        pthread_cond_signal( &serviceConnections );
    }
    return 0;
}

int http_shutdown(HTTP_Server *S) {
    if (S->nc != NULL) {
        std::cout << "Trick Webserver: Shutting down on port " << S->port << "." << std::endl;
        S->shutting_down = true;

        // Send the serviceConnections signal one last time so the connectionAttendant thread can quit.
        pthread_cond_signal( &serviceConnections );
        pthread_join(S->server_thread, NULL);
    }
    return 0;
}
