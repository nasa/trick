/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)
LIBRARY DEPENDENCIES:
    ( (../src/WSSession.o))
**************************************************************************/
#ifndef WSSESSION_HH
#define WSSESSION_HH

#include <time.h>
#include <vector>
#include <mongoose.h>
#include "WSSessionVariable.hh"

inline uint64_t to_nanoseconds(struct timespec* t) {
    return t->tv_sec * (uint64_t)1000000000L + t->tv_nsec;
}

class WSsession {

public:
    WSsession( struct mg_connection *c );
    void setTimeInterval(unsigned int milliseconds);
    void addVariable(char* vname);
    void sendValues();
    void synchSend(); // This must be called at a frequency greater than or equal to the interval.
    void pause();
    void unpause();
    void clear();
    void exit();
    int emitError(const char* fmt, ... );

    static int bad_ref_int ;

private:
    WSsession() {}
    REF2* make_error_ref(const char* in_name);
    struct mg_connection* connection;
    std::vector<WSsessionVariable*> sessionVariables;
    struct timespec lastTime;
    struct timespec interval;
    bool enabled;

};
#endif
