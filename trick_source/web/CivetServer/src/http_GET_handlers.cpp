/*************************************************************************
PURPOSE: ( HTTP-GET-method-handlers )
LIBRARY DEPENDENCIES:
    ( (../src/http_GET_handlers.o))
**************************************************************************/

#include "../include/http_GET_handlers.hh"
#include "trick/CivetServer.hh"

#include "civet/CivetServer.h"
#include "civet/civetweb.h"
#include <sstream>

#include "trick/VariableServer.hh"
extern Trick::VariableServer * the_vs ;

#include "trick/MemoryManager.hh"
extern Trick::MemoryManager* trick_MM;

static const std::string ws_api_prefix = "/api/ws";
static const std::string ws_http_prefix = "/api/http";

void http_send(struct mg_connection *conn, const char* msg, int len, int chunk_size) {
    int size = len;
    int count = 0;
    if (chunk_size > size) {
        chunk_size = len;
    }
    while (size > 0) {
        std::string buff = std::string(msg).substr(count * chunk_size, chunk_size);
        std::cout << "Sending chunk: " << buff << std::endl;
        mg_send_chunk(conn, buff.c_str(), buff.length());
        count++;
        size = size - chunk_size;
    }
    std::string buff = std::string(msg).substr(count * chunk_size, chunk_size);
    mg_send_chunk(conn, buff.c_str(), buff.length());
}

///// HTTP


int parent_http_handler(struct mg_connection* conn, void *data) {
    std::cout << "Parent handler called" << std::endl;
    MyCivetServer* server = (MyCivetServer*)data;
    const struct mg_request_info* ri = mg_get_request_info(conn);
    std::string uri = ri->local_uri_raw;
    if (uri.rfind(ws_http_prefix, 0) == 0) {
        std::string httpType = uri.substr(ws_http_prefix.size() + 1, uri.size());
        std::map<std::string, httpMethodHandler>::iterator iter;
        iter = server->httpGETHandlerMap.find(httpType);
		if (iter != server->httpGETHandlerMap.end()) {
			httpMethodHandler handler = iter->second;
			handler(conn, (void*)data);
		} else {
            mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nConnection: "
	          "close\r\nTransfer-Encoding: chunked\r\n");
	        mg_printf(conn, "Content-Type: text/plain\r\n\r\n");    
            std::stringstream ss;
            ss << "Error: http api " << httpType << " is not implemented.";
            http_send(conn, ss.str().c_str(), ss.str().size(), 100);
        }
        return 200;
    } else {
        mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nConnection: "
	          "close\r\nTransfer-Encoding: chunked\r\n");
	    mg_printf(conn, "Content-Type: text/plain\r\n\r\n");
        std::stringstream ss;
        ss << "Error: invalid url.";
        http_send(conn, ss.str().c_str(), ss.str().size(), 100);
        return 404;
    }
}

void handle_hello_world(struct mg_connection* conn, void* ignore) {
    mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nConnection: "
	          "close\r\nTransfer-Encoding: chunked\r\n");
	mg_printf(conn, "Content-Type: text/plain\r\n\r\n");
    std::string msg = "Hello world";
    http_send(conn, msg.c_str(), msg.size(), 100);
}

void handle_HTTP_GET_vs_connections(struct mg_connection* conn, void *cbdata) {
    mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nConnection: "
	          "close\r\nTransfer-Encoding: chunked\r\n");
	mg_printf(conn, "Content-Type: text/plain\r\n\r\n");
    
    std::stringstream ss;
    ss << *the_vs << std::endl;
    std::string someJSON = ss.str();

    http_send(conn, someJSON.c_str(), someJSON.length(), 100);
}

void handle_HTTP_GET_alloc_info(struct mg_connection *conn, void* ignore) {
    mg_printf(conn,
	          "HTTP/1.1 200 OK\r\nConnection: "
	          "close\r\nTransfer-Encoding: chunked\r\n");
	mg_printf(conn, "Content-Type: text/plain\r\n\r\n");
    
    const struct mg_request_info* ri = mg_get_request_info(conn);
    int max_size = 100;
    char start_str[max_size], count_str[max_size];
    mg_get_var2(ri->request_uri, strlen(ri->request_uri), "start", start_str, max_size, 0);
    mg_get_var2(ri->request_uri, strlen(ri->request_uri), "count", count_str, max_size, 0);
    mg_send_http_ok(conn, "text/plain", -1);
    std::stringstream ss;
    int start = strtol(start_str, NULL, 0);
    int count = strtol(count_str, NULL, 0);
    trick_MM->write_JSON_alloc_list(ss, start, count);
    std::string someJSON = ss.str();

    http_send(conn, someJSON.c_str(), someJSON.length(), 100);

}

///// websockets

int echo_connect_handler(const struct mg_connection *conn,
				     void *cbdata)
{
	int ret_val = 0;
	return ret_val;
}

void echo_ready_handler(struct mg_connection *conn, void *cbdata)
{
}

int echo_data_handler(struct mg_connection *conn, int bits,
				  char *data, size_t data_len, void *cbdata)
{
    std::cout << "from client:" << data << std::endl;
    mg_websocket_write(conn, MG_WEBSOCKET_OPCODE_TEXT, data, strlen(data));
    return 1;
}

void echo_close_handler(const struct mg_connection *conn,
				    void *cbdata)
{
}

int ws_connect_handler(const struct mg_connection *conn,
				     void *ignore)
{
	int ret_val = 0;
	return ret_val;
}

//VariableServer
void ws_ready_handler(struct mg_connection *conn, void *my_server)
{
	MyCivetServer* server = (MyCivetServer*) my_server;
    const struct mg_request_info* ri = mg_get_request_info(conn);
    std::string uri = ri->local_uri_raw;
    if (uri.rfind(ws_api_prefix, 0) == 0) {
        std::string wsType = uri.substr(ws_api_prefix.size() + 1, uri.size());
	    WebSocketSession* session = server->makeWebSocketSession(conn, wsType);
	    server->addWebSocketSession(conn, session);
    } else {
        std::cout << "Trick Webserver: WEBSOCKET_REQUEST: URI does not start with API prefix.\n" << std::endl;
    }
}	

int ws_data_handler(struct mg_connection *conn, int bits,
				  char *data, size_t data_len, void *my_server)
{
	int rvalue = 1;
	std::cout << "Handling message:" << data << ":" << std::endl;

	MyCivetServer* server = (MyCivetServer*) my_server;
	if (data_len > 0) { 
		// pthread_mutex_lock(&server->WebSocketSessionMapLock); //TODO: Do we need this lock?
		std::map<mg_connection*, WebSocketSession*>::iterator iter;
		iter = server->webSocketSessionMap.find(conn);
		if (iter != server->webSocketSessionMap.end()) {
			WebSocketSession* session = iter->second;
			session->handleMessage(data);
		}
		// pthread_mutex_unlock(&server->WebSocketSessionMapLock);

	} 
     return rvalue;
}

void ws_close_handler(const struct mg_connection *conn,
				    void *my_server)
{
	MyCivetServer* server = (MyCivetServer*) my_server;
	server->deleteWebSocketSession(const_cast<mg_connection*>(conn));
}