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
        WSsession(struct mg_connection *nc);

        void stageData();                                    /* -- virtual Base */
        void sendMessage();                                  /* -- virtual Base */
        int  handleMessage (std::string);                    /* -- virtual Base */

        void setTimeInterval(unsigned int milliseconds); /* -- VariableServerSession specific */
        void addVariable(char* vname);                   /* -- VariableServerSession specific */
        void stageVariableValues();                              /* -- VariableServerSession specific */
        void pause();                                    /* -- VariableServerSession specific */
        void unpause();                                  /* -- VariableServerSession specific */
        void clear();                                    /* -- VariableServerSession specific */
        void exit();                                     /* -- VariableServerSession specific */

        int emitError(const char* fmt, ... );

        static int bad_ref_int ;

    private:
        WSsession() {}

        struct mg_connection* connection;                 /* -- Base */
        
        REF2* make_error_ref(const char* in_name);        /* -- VariableServerSession specific */
        double stageTime;                                 /* -- VariableServerSession specific */
        std::vector<WSsessionVariable*> sessionVariables; /* -- VariableServerSession specific */
        bool cyclicSendEnabled;                           /* -- VariableServerSession specific */
        long long nextTime;                               /* -- VariableServerSession specific */
        long long intervalTimeTics;                       /* -- VariableServerSession specific */
};
#endif
