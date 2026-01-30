/*************************************************************************
PURPOSE: (Represent the state of a variable server websocket connection.)
**************************************************************************/
#ifndef TIMESESSION_HH
#define TIMESESSION_HH
#include "time.h"
#include "trick/WebSocketSession.hh"
#include <string>
#include <vector>

class TimeSession : public WebSocketSession
{
public:
    enum Zone
    {
        GMT,
        LOCAL
    };

    TimeSession(struct mg_connection * nc);
    ~TimeSession();
    void marshallData();
    void sendMessage();
    int handleMessage(const std::string &);

private:
    time_t now;
    Zone zone;
};

WebSocketSession * makeTimeSession(struct mg_connection * nc);
#endif
