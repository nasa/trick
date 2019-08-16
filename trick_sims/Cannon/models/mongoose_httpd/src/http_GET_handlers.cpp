/*************************************************************************
PURPOSE: ( HTTP-GET-method-handlers )
LIBRARY DEPENDENCIES:
    ( (../src/http_GET_handlers.o))
**************************************************************************/

#include <sstream>
#include "../include/http_GET_handlers.hh"

#include "trick/VariableServer.hh"
extern Trick::VariableServer * the_vs ;

#include "trick/MemoryManager.hh"
extern Trick::MemoryManager* trick_MM;

// In the Trick HTTP Server, a HTTP GET request whose URI starts with the API_PREFIX
// is processed by a http-handler-function of the following form:
//
//     void HTTP_METHOD_HANDLER( struct mg_connection *, struct http_message *);
//
// The purpose of these functions are generally to produce dynamically generated
// HTTP responses, like JSON. These handler-functions are installed into the HTTP_Server
// with the member-function <HTTP_Server-object>.install_API_GET_handler. For example:
//
//     http.server.install_API_GET_handler("vs_connections", &handle_HTTP_GET_vs_connections);
//
// installs the function handle_HTTP_GET_vs_connections() with the key "vs_connections".
// So if, for example the host and port of the webserver is "localhost:8888", and the API_PREFIX is "/api/v1/",
// then loading the URL "localhost:8888/api/v1/vs_connections" in your browser will cause
// handle_HTTP_GET_vs_connections() to run and return its response, which in this case is a JSON object
// describing the variable server connections.


// Send a JSON object to the given mongoose HTTP connection that describes the
// Variable Server Connections.
void handle_HTTP_GET_vs_connections(struct mg_connection *nc, struct http_message *hm) {
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    ss << *the_vs << std::endl;
    std::string someJSON = ss.str();
    mg_printf_http_chunk(nc, "%s", someJSON.c_str());
    mg_send_http_chunk(nc, "", 0);
}

static int getIntegerQueryValue(struct http_message *hm, const char* key, int defaultVal) {
    char value_text[100];
    if ( mg_get_http_var(&(hm->query_string), key, value_text, sizeof(value_text)) > 0) {
        return atoi(value_text);
    } else {
        return defaultVal;
    }
}

// Send a JSON object to the given mongoose HTTP connection that contains information
// about a range of memory allocations in the Trick Memory Manager.
void handle_HTTP_GET_alloc_info(struct mg_connection *nc, struct http_message *hm) {
    int start = getIntegerQueryValue(hm, "start", 0);
    int count = getIntegerQueryValue(hm, "count", 10);
    mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
    std::stringstream ss;
    trick_MM->write_JSON_alloc_list(ss, start, count);
    std::string someJSON = ss.str();
    mg_printf_http_chunk(nc, "%s", someJSON.c_str());
    mg_send_http_chunk(nc, "", 0);
}
