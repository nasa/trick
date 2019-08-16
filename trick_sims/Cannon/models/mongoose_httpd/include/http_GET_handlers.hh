/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)
LIBRARY DEPENDENCIES:
    ( (../src/http_GET_handlers.o))
**************************************************************************/
#ifndef HANDLE_HTTP_GET_HANDLERS_HH
#define HANDLE_HTTP_GET_HANDLERS_HH

#include <mongoose.h>

void handle_HTTP_GET_vs_connections(struct mg_connection *nc, struct http_message *hm);
void handle_HTTP_GET_alloc_info(struct mg_connection *nc, struct http_message *hm);

#endif
