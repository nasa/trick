/*************************************************************************
PURPOSE: (Represent the state of a variable server websocket connection.)
**************************************************************************/
#ifndef TIMESESSION_HH
#define TIMESESSION_HH
#include <vector>
#include <string>
#include "time.h"
#include "trick/WebSocketSession.hh"

class TimeSession : public WebSocketSession {
    public:
        enum Zone { GMT, LOCAL};
        TimeSession(struct mg_connection *nc);
        ~TimeSession();
        void marshallData();
        void sendMessage();
        int  handleMessage(std::string);
    private:
        time_t now;
        Zone zone;
};

WebSocketSession* makeTimeSession( struct mg_connection *nc );
#endif