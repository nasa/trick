/*************************************************************************
PURPOSE: (Represent Websocket variable server connection.)

    Messages sent from Client to Server
    ================================
    { "cmd" : "var_add",
      "var_name" : <str>
    }
    { "cmd" : "var_pause" }
    { "cmd" : "var_unpause" }
    { "cmd" : "var_send" }
    { "cmd" : "var_clear" }
    { "cmd" : "var_exit" }
    { "cmd" : "var_cycle",
      "period" : <int>
    }
    { "cmd" : "python",
      "pycode" : <str>
    }

    Messages sent from Server to Client
    =================================
    { "msg_type" : "error",
      "error_text" : <str>
    }
    { "msg_type" : "var_list"
      "time" : <double>
      "values" : []
    }
**************************************************************************/

#ifndef WSSESSION_HH
#define WSSESSION_HH

#include <vector>
#include <string>
#include <mongoose.h>
#include "WebSocketSession.hh"
#include "VariableServerVariable.hh"

class VariableServerSession : public WebSocketSession {
    public:
        VariableServerSession(struct mg_connection *nc);
        ~VariableServerSession();
        void stageData();                                /* -- base */
        void sendMessage();                              /* -- base */
        int  handleMessage(std::string);                 /* -- base */

        void setTimeInterval(unsigned int milliseconds);
        void addVariable(char* vname);
        void stageVariableValues();
        void pause();
        void unpause();
        void clear();
        void exit();

        static int bad_ref_int ;

    private:
        int sendErrorMessage(const char* fmt, ... );
        REF2* make_error_ref(const char* in_name);
        double stageTime;
        std::vector<VariableServerVariable*> sessionVariables;
        bool cyclicSendEnabled;
        long long nextTime;
        long long intervalTimeTics;
};
#endif
