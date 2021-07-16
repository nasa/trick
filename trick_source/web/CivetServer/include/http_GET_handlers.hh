/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)
LIBRARY DEPENDENCIES:
    ( (../src/http_GET_handlers.o))
**************************************************************************/
#ifndef HANDLE_HTTP_GET_HANDLERS_HH
#define HANDLE_HTTP_GET_HANDLERS_HH
#include <cstddef>

#ifndef SWIG
#include "civet/CivetServer.h"
#endif

void http_send(struct mg_connection *conn, const char* msg, int len, int chunk_size);
int http_send_ok(struct mg_connection *conn, const char* msg, int len, int chunk_size);
int http_send_error(struct mg_connection *conn, int error_code, const char* msg, int len, int chunk_size);

int parent_http_handler(struct mg_connection* conn, void *cbdata);

void handle_HTTP_GET_vs_connections(struct mg_connection *nc, void* cbdata);
void handle_HTTP_GET_alloc_info(struct mg_connection *nc, void* ignore);

int echo_connect_handler(const struct mg_connection *conn,
				     void *cbdata);

void echo_ready_handler(struct mg_connection *conn, void *cbdata);

int echo_data_handler(struct mg_connection *conn, int bits,
				  char *data, size_t data_len, void *cbdata);

void echo_close_handler(const struct mg_connection *conn,
				    void *cbdata);

int ws_connect_handler(const struct mg_connection *conn,
				     void *ignore);

//VariableServer
void ws_ready_handler(struct mg_connection *conn, void *my_server);

int ws_data_handler(struct mg_connection *conn, int bits,
				  char *data, size_t data_len, void *my_server);

void ws_close_handler(const struct mg_connection *conn,
				    void *my_server);

int begin_request(struct mg_connection* conn);

#endif
