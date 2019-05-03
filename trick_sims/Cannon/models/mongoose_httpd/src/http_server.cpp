/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
**************************************************************************/
#include "../include/http_server.h"
#include <sstream>

#include "trick/VariableServer.hh"
extern Trick::VariableServer * the_vs ;
#include "trick/MemoryManager.hh"
extern Trick::MemoryManager* trick_MM;

static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delele_method = MG_MK_STR("DELETE");

int getIntegerQueryValue(struct http_message *hm, const char* key, int defaultVal) {
    char value_text[100];
    if ( mg_get_http_var(&(hm->query_string), key, value_text, sizeof(value_text)) > 0) {
        return atoi(value_text);
    } else {
        return defaultVal;
    }
}

void handle_vs_connections_call(struct mg_connection *nc, struct http_message *hm) {
    /* Send headers */
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    ss << *the_vs << std::endl;
    std::string tmp = ss.str();
    mg_printf_http_chunk(nc, "%s", tmp.c_str());
    mg_send_http_chunk(nc, "", 0);
}

void handle_alloc_info_call(struct mg_connection *nc, struct http_message *hm) {
    int start = getIntegerQueryValue(hm, "start", 0);
    int count = getIntegerQueryValue(hm, "count", 10);
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    trick_MM->write_JSON_alloc_list(ss, start, count);
    std::string tmp = ss.str();
    mg_printf_http_chunk(nc, "%s", tmp.c_str());
    mg_send_http_chunk(nc, "", 0);
}

static int has_prefix(const struct mg_str *uri, const struct mg_str *prefix) {
  return uri->len > prefix->len && memcmp(uri->p, prefix->p, prefix->len) == 0;
}

static int is_equal(const struct mg_str *s1, const struct mg_str *s2) {
  return s1->len == s2->len && memcmp(s1->p, s2->p, s2->len) == 0;
}

static struct mg_serve_http_opts http_server_options;
static const struct mg_str api_prefix = MG_MK_STR("/api/v1");

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {

    http_message *hm = (struct http_message *)ev_data;

    switch(ev) {
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
            printf("DEBUG: Event MG_EV_WEBSOCKET_HANDSHAKE_DONE.\n");
            const char* s = "Bingo boingo.";
            mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, s, strlen(s));
        } break;
        case MG_EV_WEBSOCKET_FRAME: {
            printf("DEBUG: Event MG_EV_WEBSOCKET_FRAME.\n");
        } break;
        case MG_EV_CONNECT: {
            printf("DEBUG: Event MG_EV_CONNECT.\n");
        } break;
        // case MG_EV_RECV: {
        //     printf("DEBUG: Event MG_EV_RECV.\n");
        // } break;
        // case MG_EV_SEND: {
        //     printf("DEBUG: Event MG_EV_SEND.\n");
        // } break;
        case MG_EV_CLOSE: {
            printf("DEBUG: Event MG_EV_CLOSE.\n");
        } break;
        // case MG_EV_POLL: {
        //     printf("DEBUG: Event MG_EV_POLL.\n");
        // } break;
        case MG_EV_HTTP_REQUEST: {
            printf("DEBUG: Event MG_EV_HTTP_REQUEST.\n");
            char * s = strndup(hm->uri.p, hm->uri.len);
            printf("DEBUG: URI = \"%s\"\n", s);
            free(s);
            if (has_prefix(&hm->uri, &api_prefix)) {
                struct mg_str key;
                key.p = hm->uri.p + api_prefix.len;
                key.len = hm->uri.len - api_prefix.len;

                if (is_equal(&hm->method, &s_get_method)) {
                    printf("DEBUG: HTTP GET method.\n");
                    if (mg_vcmp(&key, "/vs_connections") == 0) {
                        handle_vs_connections_call(nc, hm);
                    } else if (mg_vcmp(&key, "/alloc_info") == 0) {
                        handle_alloc_info_call(nc, hm);
                    }
                } else if (is_equal(&hm->method, &s_put_method)) {
                    printf("DEBUG: HTTP PUT method.\n");
                } else if (is_equal(&hm->method, &s_delele_method)) {
                    printf("DEBUG: HTTP DELETE method.\n");
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
    while(!S->shutting_down) {
        mg_mgr_poll(&S->mgr, 1000);
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
