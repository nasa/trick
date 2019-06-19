/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
LIBRARY DEPENDENCIES:
    ((WSSession.o))
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
  "values" : []
}

LEXEMES:
    {
    }
    "[^""]"
    :
    ,
    [0-9]+
}
*/
#include "../include/http_server.h"
#include <sstream>
#include <unistd.h>
#include "../include/WSSession.hh"

#include "trick/VariableServer.hh"
extern Trick::VariableServer * the_vs ;
#include "trick/MemoryManager.hh"
extern Trick::MemoryManager* trick_MM;

static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delele_method = MG_MK_STR("DELETE");

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
  return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

int getIntegerQueryValue(struct http_message *hm, const char* key, int defaultVal) {
    char value_text[100];
    if ( mg_get_http_var(&(hm->query_string), key, value_text, sizeof(value_text)) > 0) {
        return atoi(value_text);
    } else {
        return defaultVal;
    }
}

static const struct mg_str api_prefix = MG_MK_STR("/api/v1");

// Respond to HTTP GET with URI="/api/v1/vs_connections".
void handle_HTTP_GET_vs_connections(struct mg_connection *nc, struct http_message *hm) {
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    ss << *the_vs << std::endl;
    std::string tmp = ss.str();
    mg_printf_http_chunk(nc, "%s", tmp.c_str());
    mg_send_http_chunk(nc, "", 0);
}

// Respond to HTTP GET with URI="/api/v1/alloc_info".
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

// =============================================================================

static struct mg_serve_http_opts http_server_options;
std::map<mg_connection*, WSsession*> wsSessionMap;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {

    http_message *hm = (struct http_message *)ev_data;

    switch(ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
#ifdef DEBUG
            printf("DEBUG: Event MG_EV_WEBSOCKET_HANDSHAKE_DONE. nc = %p.\n", nc);
#endif
            WSsession* session = new WSsession(nc);

            session->setTimeInterval(500);            // TEMPORARY for development
            session->addVariable("dyn.cannon.pos[0]");// TEMPORARY for development
            session->addVariable("dyn.cannon.pos[1]");// TEMPORARY for development
            session->unpause();                       // TEMPORARY for development

            wsSessionMap.insert( std::pair<mg_connection*, WSsession*>(nc, session) );
        } break;
        case MG_EV_WEBSOCKET_FRAME: {
            // Process messages recieved from the (web browser) client.
            struct websocket_message *wm = (struct websocket_message *) ev_data;

            char* msg = strndup((char*)wm->data, wm->size);
#ifdef DEBUG
            printf("DEBUG: Even t MG_EV_WEBSOCKET_FRAME: nc = %p message = %s\n", nc, msg);
#endif
            free(msg);

        } break;
        case MG_EV_CONNECT: {
#ifdef DEBUG
            printf("DEBUG: Event MG_EV_CONNECT. nc = %p.\n", nc);
#endif
        } break;
        case MG_EV_CLOSE: {
#ifdef DEBUG
            printf("DEBUG: Event MG_EV_CLOSE. nc = %p.\n", nc);
#endif
            if (nc->flags & MG_F_IS_WEBSOCKET) {
                std::map<mg_connection*, WSsession*>::iterator iter;
                iter = wsSessionMap.find(nc);
                if (iter != wsSessionMap.end()) {
                    WSsession* session = iter->second;
                    delete session;
                    wsSessionMap.erase(iter);
                }
            }
        } break;
        case MG_EV_POLL: {
#ifdef DEBUG
           printf("DEBUG: Event MG_EV_POLL. nc = %p.\n", nc);
#endif
           if (nc->flags & MG_F_IS_WEBSOCKET) {
               std::map<mg_connection*, WSsession*>::iterator iter;
               // Find the session that goes with this connection.
               iter = wsSessionMap.find(nc);
               if (iter != wsSessionMap.end()) {
                   WSsession* session = iter->second;
                   session->synchSend();
               }
           }
        } break;
        case MG_EV_HTTP_REQUEST: {
#ifdef DEBUG
            printf("DEBUG: Event MG_EV_HTTP_REQUEST.\n");
#endif
            char * s = strndup(hm->uri.p, hm->uri.len);
#ifdef DEBUG
            printf("DEBUG: URI = \"%s\"\n", s);
#endif
            free(s);
            if (has_prefix(&hm->uri, &api_prefix)) {
                struct mg_str key;
                key.p = hm->uri.p + api_prefix.len;
                key.len = hm->uri.len - api_prefix.len;

                if (is_equal(&hm->method, &s_get_method)) {
#ifdef DEBUG
                    printf("DEBUG: HTTP GET method.\n");
#endif
                    if (mg_vcmp(&key, "/vs_connections") == 0) {
                        handle_HTTP_GET_vs_connections(nc, hm);
                    } else if (mg_vcmp(&key, "/alloc_info") == 0) {
                        handle_HTTP_GET_alloc_info(nc, hm);
                    }
                } else if (is_equal(&hm->method, &s_put_method)) {
#ifdef DEBUG
                    printf("DEBUG: HTTP PUT method.\n");
#endif
                } else if (is_equal(&hm->method, &s_delele_method)) {
#ifdef DEBUG
                    printf("DEBUG: HTTP DELETE method.\n");
#endif
                }
            } else {
               mg_serve_http(nc, (struct http_message *) ev_data, http_server_options);
            }
        } break;
        default: {
        } break;
    }
}

void* service_connections (void* arg) {
    HTTP_Server *S = (HTTP_Server*)arg;
    while (!S->shutting_down) {
        mg_mgr_poll(&S->mgr, 1000);
        struct timespec nap, t2;
        nap.tv_sec  = 0;
        nap.tv_nsec = 100000000L;
        if ( nanosleep(&nap, &t2) < 0) {
            // Error
        };
    }
    return NULL;
}

int http_default_data(HTTP_Server *S) {
    S->port = "8888";
    S->shutting_down = false;
    return 0;
}

int http_init(HTTP_Server *S) {
    http_server_options.document_root = "www";
    http_server_options.enable_directory_listing = "yes";
    mg_mgr_init(&S->mgr, NULL);
    printf("Starting web server on port %s\n", S->port);
    S->nc = mg_bind(&S->mgr, S->port, ev_handler);
    if (S->nc == NULL) {
      printf("Failed to create listener.\n");
      return 1;
    }
    mg_set_protocol_http_websocket(S->nc);
    pthread_create( &S->server_thread, NULL, service_connections, (void*)S);
    return 0;
}

int http_shutdown(HTTP_Server *S) {
    printf("Shutting down web server on port %s\n", S->port);
    S->shutting_down = true;
    pthread_join(S->server_thread, NULL);
    return 0;
}
