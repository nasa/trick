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
    WSsession( struct mg_connection *nc);
    void setTimeInterval(unsigned int milliseconds);
    void addVariable(char* vname);
    void stageValuesSynchronously();
    void stageValues();
    void sendValues();
    void pause();
    void unpause();
    void clear();
    void exit();
    int handle_msg (const char* client_msg);
    int emitError(const char* fmt, ... );

    static int bad_ref_int ;

private:
    WSsession() {}
    REF2* make_error_ref(const char* in_name);
    struct mg_connection* connection;
    std::vector<WSsessionVariable*> sessionVariables;
    bool cyclicSendEnabled;
    double stageTime;
    bool valuesStaged;
    long long nextTime;
    long long intervalTimeTics;


};
#endif
