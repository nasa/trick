/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)
LIBRARY DEPENDENCIES:
    ( (../src/WSSession.o))
**************************************************************************/
#ifndef WSSESSION_HH
#define WSSESSION_HH

#include <vector>
#include <string>
#include <mongoose.h>
#include "WSSessionVariable.hh"

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
        int handle_msg (std::string);
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
