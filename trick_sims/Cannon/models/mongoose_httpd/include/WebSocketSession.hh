/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)
LIBRARY DEPENDENCIES:
    ( (../src/WSSession.o))
**************************************************************************/
#ifndef WEB_SOCKET_SESSION_HH
#define WEB_SOCKET_SESSION_HH

#include <string>
#include <mongoose.h>

class WebSocketSession {
    public:
        WebSocketSession(struct mg_connection *nc):connection(nc){};
        virtual ~WebSocketSession() {};
        virtual void stageData()=0;
        virtual void sendMessage()=0;
        virtual int  handleMessage(std::string)=0;

        struct mg_connection* connection;
};
#endif
