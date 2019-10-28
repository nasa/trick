/************************************************************************
PURPOSE: (Represent the state and initial conditions of an http server)
**************************************************************************/
#include <sys/stat.h> // for mkdir()
#include <unistd.h>   // for symlink(), access()
#include <stdlib.h>   // for getenv()
#include <dirent.h>   // for opendir(), readdir()
#include <iostream>
#include <fstream>
#include "trick/WebServer.hh"
#include "../include/http_GET_handlers.hh"
#include "../include/VariableServerSession.hh"
#include <string.h>
#include <string>
static const struct mg_str s_get_method = MG_MK_STR("GET");
static const struct mg_str s_put_method = MG_MK_STR("PUT");
static const struct mg_str s_delete_method = MG_MK_STR("DELETE");
static const struct mg_str http_api_prefix = MG_MK_STR("/api/http/");
static const struct mg_str ws_api_prefix = MG_MK_STR("/api/ws/");

static const char * style_css =
    "h1 {"
        "font-family: fantasy, cursive, serif;"
        "font-size: 32px;"
        "margin-left: 1em;"
    "}"
    "h2 {"
        "font-family: sans-serif;"
        "font-size: 18px;"
        "margin-left: 1em;"
    "}"
    "a {"
        "font-family: sans-serif;"
        "font-size: 16px;"
    "}"
    "div.header { background-image: linear-gradient(#afafff, white); }";

static const char * index_html =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
        "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n"
        "<title>Trick Simulation</title>\n"
        "<div class=\"header\">\n"
            "<table>\n"
                "<th><img src=\"images/trick_icon.png\" height=\"64\" width=\"64\"></th>\n"
                "<th><h1>Trick Simulation</h1></th>\n"
            "</table>\n"
        "</div>\n"
    "</head>\n"
    "<body>\n"
    "<div style=\"background:#efefef\">\n"
      "<ul>\n"
      "<li><a href=\"http://github.com/nasa/trick\">Trick on GitHub</a></li>\n"
      "<li><a href=\"http://github.com/nasa/trick/wiki/Tutorial\">Trick Tutorial</a></li>\n"
      "<li><a href=\"http://github.com/nasa/trick/wiki/Documentation-Home\">Trick Documentation</a></li>\n"
      "</ul>\n"
    "</div>\n"
    "<div style=\"background:#efefef\">\n"
      "<ul>\n"
      "<li><a href=\"/apps\">Applications</a></li>\n"
      "</ul>\n"
    "</div>\n"
    "</body>\n"
    "</html>";

static int confirmDocumentRoot ( std::string documentRoot ) {

    if ( access( documentRoot.c_str(), F_OK ) != -1 ) {
        std::cout << "Trick Webserver: Document root \"" << documentRoot
                  << "\" already exists." << std::endl;
    } else {
        std::cout << "Trick Webserver: Document root \"" << documentRoot
                  << "\" doesn't exist so, we'll create it."
                  << std::endl;

    	char* trick_home = getenv("TRICK_HOME");
        std::string trickHome = std::string(trick_home);

        if (trick_home != NULL) {
            if ( mkdir( documentRoot.c_str(), 0700) == 0) {

                 std::string styleFilePath = documentRoot + "/style.css";
                 std::fstream style_fs (styleFilePath, std::fstream::out);
                 style_fs << style_css << std::endl;
                 style_fs.close();

                 std::string appsDirPath = documentRoot + "/apps";
                 if ( mkdir( appsDirPath.c_str(), 0700) == 0) {
                     DIR *dr;
                     struct dirent * dir_entry;
                     std::string trickAppsDirPath = trickHome + "/trick_source/web/apps";
                     if ( (dr = opendir(trickAppsDirPath.c_str())) != NULL) {
                         while (( dir_entry = readdir(dr)) != NULL) {
                             std::string fName = std::string( dir_entry->d_name);
                             std::string sPath = trickAppsDirPath + '/' + fName;
                             std::string dPath = appsDirPath + '/' + fName;
                             symlink(sPath.c_str(), dPath.c_str());
                         }
                     }
                 } else {
                     std::cout << "Failed to create \"" << appsDirPath << "\"." << std::endl;
                     return 1;
                 }

                 std::string imagesDirPath = documentRoot + "/images";
                 if ( mkdir( imagesDirPath.c_str(), 0700) == 0) {
                     DIR *dr;
                     struct dirent * dir_entry;
                     std::string trickImagesDirPath = trickHome + "/trick_source/web/images";
                     if ( (dr = opendir(trickImagesDirPath.c_str())) != NULL) {
                         while (( dir_entry = readdir(dr)) != NULL) {
                             std::string fName = std::string( dir_entry->d_name);
                             std::string sPath = trickImagesDirPath + '/' + fName;
                             std::string dPath = imagesDirPath + '/' + fName;
                             symlink(sPath.c_str(), dPath.c_str());
                         }
                     }
                 } else {
                     std::cout << "Failed to create \"" << imagesDirPath << "\"." << std::endl;
                     return 1;
                 }

                 std::string indexFilePath = documentRoot + "/index.html";
                 std::fstream index_fs (indexFilePath, std::fstream::out);
                 index_fs << index_html << std::endl;
                 index_fs.close();

             } else {
                 std::cout << "Failed to create documentRoot." << std::endl;
                 return 1;
             }
        } else {
             std::cout << "TRICK_HOME is not set.\n" << std::endl;
             return 1;
        }
    }
    return 0;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {

    http_message *hm = (struct http_message *)ev_data;
    WebServer* httpServer = (WebServer *)nc->user_data;

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
            } else {
                std::cout << "ERROR: WEBSOCKET_REQUEST URI does not start with API prefix."  << std::endl;
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
           // called periodically by the threaded function connectionAttendant() [below] when it is
           // signaled (serviceConnections) from the http_top_of_frame job.
           // This is when we send websocket messages to the client (web browser).
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
   WebServer *S = (WebServer*)arg;
   while(1) {
        pthread_mutex_lock(&S->serviceLock);
        // Wait here until the serviceConnections condition is signaled by the top_of_frame job.
        while (!S->service_websocket && !S->shutting_down) {
            pthread_cond_wait(&S->serviceConnections, &S->serviceLock);
        }
        if (S->shutting_down) {
            pthread_mutex_unlock(&S->serviceLock);
            return NULL;
        } else {
            if (!S->sessionDataMarshalled) {
                S->marshallWebSocketSessionData();
            }
            // mg_mgr_poll returns the number of connections that still need to be serviced.
            while(mg_mgr_poll(&S->mgr, 50));
        }
        S->service_websocket= false;
        pthread_mutex_unlock(&S->serviceLock);
    }
    return NULL;
}

// Install a WebSocketSessionMaker with a name (key) by which it can be retrieved.
void WebServer::installWebSocketSessionMaker(std::string name, WebSocketSessionMaker maker) {
    pthread_mutex_lock(&WebSocketSessionMakerMapLock);
    WebSocketSessionMakerMap.insert(std::pair<std::string, WebSocketSessionMaker>(name, maker));
    pthread_mutex_unlock(&WebSocketSessionMakerMapLock);
}

// Lookup and call the WebSocketSessionMaker function by name, end execute it to create and return
// (a pointer to) a WebSocketSession.
WebSocketSession* WebServer::makeWebSocketSession(struct mg_connection *nc, std::string name) {
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

// Install an httpMethodHandler with a name, the key by which it can be retrieved.
void WebServer::installHTTPGEThandler(std::string handlerName, httpMethodHandler handler) {
    pthread_mutex_lock(&httpGETHandlerMapLock);
    httpGETHandlerMap.insert(std::pair<std::string, httpMethodHandler>(handlerName, handler));
    pthread_mutex_unlock(&httpGETHandlerMapLock);
}

/* Lookup the appropriate httpMethodHandler by name, and execute it for the
   given connection and http_message. */
void WebServer::handleHTTPGETrequest(struct mg_connection *nc, http_message *hm, std::string handlerName) {
    std::map<std::string, httpMethodHandler>::iterator iter;
    iter = httpGETHandlerMap.find(handlerName);
    if (iter != httpGETHandlerMap.end()) {
        httpMethodHandler handler = iter->second;
        handler(nc, hm);
    } else {
       mg_http_send_error(nc, 404, "No such API.");
    }
}

/* Tell each of the sessions to marshall any data that they have to send. */
void WebServer::marshallWebSocketSessionData() {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    pthread_mutex_lock(&webSocketSessionMapLock);
    for (iter = webSocketSessionMap.begin(); iter != webSocketSessionMap.end(); iter++ ) {
        WebSocketSession* session = iter->second;
        session->marshallData();
    }
    sessionDataMarshalled = true;
    pthread_mutex_unlock(&webSocketSessionMapLock);
}

// Find the session that goes with the given websocket connection,
// and tell it to send any messages it may have, to the client (web browser).
void WebServer::sendWebSocketSessionMessages(struct mg_connection *nc) {

    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    pthread_mutex_lock(&webSocketSessionMapLock);
    iter = webSocketSessionMap.find(nc);
    if (iter != webSocketSessionMap.end()) {
        WebSocketSession* session = iter->second;
        session->sendMessage();
    }
    sessionDataMarshalled = false;
    pthread_mutex_unlock(&webSocketSessionMapLock);
}

/* Delete the WebSocketSession associated with the given connection-pointer,
   and erase its pointer from the webSocketSessionMap. */
void WebServer::deleteWebSocketSession(struct mg_connection *nc) {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    iter = webSocketSessionMap.find(nc);
    if (iter != webSocketSessionMap.end()) {
        WebSocketSession* session = iter->second;
        delete session;
        webSocketSessionMap.erase(iter);
    }
}

// Lookup the WebSocketSession associated with the given connection-pointer, and pass
// the given message to it.
void WebServer::handleWebSocketClientMessage(struct mg_connection *nc, std::string msg) {
    std::map<mg_connection*, WebSocketSession*>::iterator iter;
    iter = webSocketSessionMap.find(nc);
    if (iter != webSocketSessionMap.end()) {
        WebSocketSession* session = iter->second;
        session->handleMessage(msg);
    }
}

// Install a WebSocketSession with a connection-pointer, the key by which it can be retrieved.
void WebServer::addWebSocketSession(struct mg_connection *nc, WebSocketSession* session) {
    pthread_mutex_lock(&webSocketSessionMapLock);
    webSocketSessionMap.insert( std::pair<mg_connection*, WebSocketSession*>(nc, session) );
    pthread_mutex_unlock(&webSocketSessionMapLock);
}

// =========================================================================
// Trick Sim Interface Functions
// =========================================================================

// Trick "default_data" job.
int WebServer::http_default_data() {
    port = "8888";
    //port = "0";
    document_root = "www";
    time_homogeneous = false;
    service_websocket = false;
    shutting_down = false;
    sessionDataMarshalled = false;
    listener = NULL;
    enable = false;

    installHTTPGEThandler("vs_connections", &handle_HTTP_GET_vs_connections);
    installHTTPGEThandler("alloc_info", &handle_HTTP_GET_alloc_info);
    installWebSocketSessionMaker("VariableServer", &makeVariableServerSession);

    return 0;
}

// Trick "initialization" job.
int WebServer::http_init() {

    if (enable) {
        http_server_options.document_root = document_root;
        http_server_options.enable_directory_listing = "yes";

        confirmDocumentRoot( std::string(document_root));

        mg_mgr_init( &mgr, NULL );

        memset(&bind_opts, 0, sizeof(bind_opts));
        bind_opts.user_data = this;
        listener = mg_bind_opt( &mgr, port, ev_handler, bind_opts);

        // Determine the ACTUAL listen port as opposed to the requested port.
        // Note that if we specify port = "0" in the mg_bind_opt() call, then
        // a port number will be chosen for us, and we have to find out what it actually is.
        char buf[32];
        mg_conn_addr_to_str( listener, buf, 32, MG_SOCK_STRINGIFY_PORT);
        port = strdup(buf);

        if (listener != NULL) {
            std::cout << "Trick Webserver: Listening on port = " << port << ".\n"
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
    } else {
        std::cout << "Trick Webserver: Disabled."
                  << std::endl;
    }
    return 0;
}

int WebServer::http_top_of_frame() {
    if (listener != NULL) {
        if (time_homogeneous) {
            /* Have all of the sessions stage their data in a top_of_frame job, so
    ``           that it's time-homogeneous. */
            marshallWebSocketSessionData();
        }
        // Signal the server thread to construct and send the values-message to the client.
        service_websocket= true;
        pthread_cond_signal( &serviceConnections );
    }
    return 0;
}

int WebServer::http_shutdown() {
    if (listener != NULL) {
        std::cout << "Trick Webserver: Shutting down on port " << port << "." << std::endl;
        shutting_down = true;

        // Send the serviceConnections signal one last time so the connectionAttendant thread can quit.
        pthread_cond_signal( &serviceConnections );
        pthread_join(server_thread, NULL);
    }
    return 0;
}
