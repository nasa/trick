/*************************************************************************
PURPOSE: (Represent the state of a variable server websocket connection.)
**************************************************************************/

#ifndef WSSESSION_HH
#define WSSESSION_HH

#include <vector>
#include <string>

#ifndef SWIG
#include "civet/CivetServer.h"
#endif

#include "trick/WebSocketSession.hh"
#include "VariableServerVariable.hh"

class VariableServerSession : public WebSocketSession {
    public:
        VariableServerSession(struct mg_connection *nc);
        ~VariableServerSession();
        void marshallData();                             /* -- base */
        void sendMessage();                              /* -- base */
        int  handleMessage(std::string);                 /* -- base */

        void setTimeInterval(unsigned int milliseconds);
        void addVariable(char* vname);
        void stageValues();
        void pause();
        void unpause();
        void clear();
        void exit();

        static int bad_ref_int ;

    private:
        int sendErrorMessage(const char* fmt, ... );
        int sendSieMessage(void);
        int sendUnitsMessage(const char* vname);
        REF2* make_error_ref(const char* in_name);
        double stageTime;
        bool dataStaged;

        std::vector<VariableServerVariable*> sessionVariables;
        bool cyclicSendEnabled;
        long long nextTime;
        long long intervalTimeTics;
};

WebSocketSession* makeVariableServerSession( struct mg_connection *nc );
#endif
