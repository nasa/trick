/*************************************************************************
PURPOSE: (Represent Websocket connection.)
**************************************************************************/
#ifndef WEB_SOCKET_SESSION_HH
#define WEB_SOCKET_SESSION_HH

#ifdef USE_CIVET
#include <string>
#ifndef SWIG
#include "civet/CivetServer.h"
#endif

class WebSocketSession {
    public:
        WebSocketSession(struct mg_connection *nc):connection(nc){};
        virtual ~WebSocketSession() {};

        /**
           When HTTP_Server::time_homogeneous is set, WebSocketSession::marshallData() is called from the main
           sim thread in a "top_of_frame" job, so that all of the data can be staged at
           the same sim-time, in other words it's time-homogeneous.
        */
        virtual void marshallData()=0;
        virtual void sendMessage()=0;
        virtual int  handleMessage(std::string)=0;

        struct mg_connection* connection;
};

#endif
#endif