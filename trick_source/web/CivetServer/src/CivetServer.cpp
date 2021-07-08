/************************************************************************
PURPOSE: (Represent the state and initial conditions for my server)
**************************************************************************/


#include <sys/stat.h> // for mkdir()
#include <unistd.h>   // for symlink(), access()
#include <stdlib.h>   // for getenv()
#include <dirent.h>   // for opendir(), readdir()
#include <iostream>
#include <fstream>
#include "trick/CivetServer.hh"
#include <string.h>
#include <string>

#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/input_processor_proto.h"
#include "trick/exec_proto.h"
#include "../include/simpleJSON.hh"
#include "../include/VariableServerSession.hh"
#include "trick/WebSocketSession.hh"

#include "civet/CivetServer.h"
#include "civet/civetweb.h"
#include "../include/http_GET_handlers.hh"

pthread_mutex_t lock_requests;

void MyCivetServer::deleteWebSocketSession(struct mg_connection * nc) {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    iter = webSocketSessionMap.find(nc);
    if (iter != webSocketSessionMap.end()) {
        WebSocketSession* session = iter->second;
        delete session;
        webSocketSessionMap.erase(iter);
    }
}

void* start_civet(void* obj)
{
	MyCivetServer* server = (MyCivetServer*)obj;

	mg_init_library(0);

	struct mg_callbacks callbacks;
	memset(&callbacks, 0, sizeof(callbacks));
    callbacks.begin_request = begin_request;

	std::string port = std::to_string(server->port);
	const char*options[] = {
		"listening_ports", port.c_str(), "document_root", "www", 0
	};

	server->ctx = mg_start(&callbacks, 0, options);

	if (server->ctx == NULL) {
		std::cout << "ERROR: Could not create server." << std::endl;
	}


    mg_set_request_handler(server->ctx, "/api/http", parent_http_handler, (void*)server);
	mg_set_websocket_handler(server->ctx, "/api/ws/VariableServer", ws_connect_handler, ws_ready_handler, ws_data_handler, ws_close_handler, obj);

}

WebSocketSession* MyCivetServer::makeWebSocketSession(mg_connection *nc, std::string name) {
    std::map<std::string, WebSocketSessionMaker>::iterator iter;
    iter = WebSocketSessionMakerMap.find(name);
    if (iter != WebSocketSessionMakerMap.end()) {
        WebSocketSessionMaker maker = iter->second;
        return maker(nc);
    } else {
        return NULL;
       mg_websocket_write(nc, MG_WEBSOCKET_OPCODE_TEXT, "ERROR: Could not create web socket session", 0);
    }
}

int MyCivetServer::default_data() {
    port = 8888;
    enable = true;
    debug = true;
	sessionDataMarshalled = false;

    installWebSocketSessionMaker("VariableServer", makeVariableServerSession);
    installHTTPGEThandler("test", handle_hello_world);
    installHTTPGEThandler("vs_connections", handle_HTTP_GET_vs_connections);
    installHTTPGEThandler("alloc_info", handle_HTTP_GET_alloc_info);
    
    return 0;
}

void MyCivetServer::installHTTPGEThandler(std::string handlerName, httpMethodHandler handler) {
    pthread_mutex_lock(&httpGETHandlerMapLock);
    httpGETHandlerMap.insert(std::pair<std::string, httpMethodHandler>(handlerName, handler));
    pthread_mutex_unlock(&httpGETHandlerMapLock);
}

void MyCivetServer::installWebSocketSessionMaker(std::string name, WebSocketSessionMaker maker) {
    pthread_mutex_lock(&WebSocketSessionMakerMapLock);
    WebSocketSessionMakerMap.insert(std::pair<std::string, WebSocketSessionMaker>(name, maker));
    pthread_mutex_unlock(&WebSocketSessionMakerMapLock);
}

void MyCivetServer::addWebSocketSession(struct mg_connection *nc, WebSocketSession* session) {
    pthread_mutex_lock(&WebSocketSessionMapLock);
    webSocketSessionMap.insert( std::pair<mg_connection*, WebSocketSession*>(nc, session) );
    pthread_mutex_unlock(&WebSocketSessionMapLock);
}

void* main_loop(void* S) {
	pthread_t civet_thread;
	MyCivetServer* server = (MyCivetServer*) S;
	bool messageSent;
	int rc = pthread_create(&civet_thread, NULL, start_civet, S);
	if (rc) {
		std::cout << "Error:unable to create thread," << rc << std::endl;
		exit(-1);
	}

	std::cout << "Starting main loop" << std::endl;
	while(1) {
		pthread_mutex_lock(&server->lock_loop);
        pthread_mutex_unlock(&lock_requests);
        // std::cout << "Entering loop." << std::endl;
		if (!server->sessionDataMarshalled) {
			server->marshallWebSocketSessionData();
		}

		std::map<mg_connection*, WebSocketSession*>::iterator iter;
		messageSent = false;
		pthread_mutex_lock(&server->WebSocketSessionMapLock);
		for (iter = server->webSocketSessionMap.begin(); iter != server->webSocketSessionMap.end(); iter++ ) {
			struct mg_connection* conn = iter->first;
			WebSocketSession* session = iter->second;
			session->sendMessage();
			messageSent = true;
		}
		if (messageSent) {
			server->sessionDataMarshalled = false;
		}
		pthread_mutex_unlock(&server->WebSocketSessionMapLock);
	}

	pthread_join(civet_thread, NULL);
}


int MyCivetServer::init() {
    if (enable) {
         int rc;

        std::cout << "Init MyCivetServer..." << std::endl;
        rc = pthread_create(&server_thread, NULL, main_loop, (void*)this);
        if (rc) {
            std::cout << "Error:unable to create thread," << rc << std::endl;
                exit(-1);
        }
        std::cout << "Finished init.  Server is now listening" << std::endl;

    } else {
        std::cout << "Not starting my server because it is not enabled." << std::endl;
    }
    return 0;
}

std::vector<std::string> split(std::string s, std::string delim) {
    std::vector<std::string> values;
    auto start = 0;
    auto end = s.find(delim);
    while (end != std::string::npos)
    {
        values.push_back(s.substr(start, end - start));
        start = end + delim.length();
        end = s.find(delim, start);
    }
    values.push_back(s.substr(start, end - start));
    return values;
}

int MyCivetServer::http_top_of_frame() {
    if (ctx != NULL) {
		// marshallWebSocketSessionData(); //TODO: Only do this if time_homogenous is on.
		unlockConnections();
	}
    return 0;
}

void MyCivetServer::unlockConnections() {
	pthread_mutex_unlock(&lock_loop);
    // std::map<mg_connection*, WebSocketSession*>::iterator iter;
    // pthread_mutex_lock(&WebSocketSessionMapLock);
    // for (iter = webSocketSessionMap.begin(); iter != webSocketSessionMap.end(); iter++ ) {
    //     WebSocketSession* session = iter->second;
	// 	mg_unlock_connection(session->connection);
    // }
    // sessionDataMarshalled = true;
    // pthread_mutex_unlock(&WebSocketSessionMapLock);
}

void MyCivetServer::sendWebSocketSessionMessages(struct mg_connection *nc) {

    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    pthread_mutex_lock(&WebSocketSessionMapLock);
    iter = webSocketSessionMap.find(nc);
    if (iter != webSocketSessionMap.end()) {
        WebSocketSession* session = iter->second;
        session->sendMessage();
    }
    sessionDataMarshalled = false;
    pthread_mutex_unlock(&WebSocketSessionMapLock);
}

void MyCivetServer::marshallWebSocketSessionData() {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    pthread_mutex_lock(&WebSocketSessionMapLock);
    for (iter = webSocketSessionMap.begin(); iter != webSocketSessionMap.end(); iter++ ) {
        WebSocketSession* session = iter->second;
        session->marshallData();
    }
    sessionDataMarshalled = true;
    pthread_mutex_unlock(&WebSocketSessionMapLock);
}

int MyCivetServer::shutdown() {
    if (enable) {
        std::cout << "Closing server." << std::endl;
		mg_stop(ctx);
		mg_exit_library();
        // join();
    }
    return 0;
}

int MyCivetServer::join() {
    pthread_join(server_thread, NULL);
    return 0;
}


pthread_mutex_t conn_map_lock;
std::map<struct mg_connection*, int> g_conn_map;
int last_conn_id = 0;

int begin_request(struct mg_connection* conn) {
    pthread_mutex_lock(&lock_requests);

    pthread_mutex_lock(&conn_map_lock);
	g_conn_map.insert(std::pair<struct mg_connection*, int>(conn, last_conn_id));
    last_conn_id++;
    pthread_mutex_unlock(&conn_map_lock);
    
    std::map<struct mg_connection*, int>::iterator iter;
    iter = g_conn_map.find(conn);
    int id = iter->second;
    std::cout << "Processing request: " << id << std::endl;
    return 0;
}