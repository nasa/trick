/*************************************************************************
PURPOSE: (Represent Websocket variable server variable.)
LIBRARY DEPENDENCIES:
    ( (../src/VariableServerVariable.o))
**************************************************************************/
#ifndef VARIABLE_SERVER_VARIABLE_HH
#define VARIABLE_SERVER_VARIABLE_HH

#include <time.h>
#include <vector>
#include <mongoose.h>
#include <iostream>
#include <trick/reference.h>

#define MAX_ARRAY_LENGTH 4096

class VariableServerVariable {

    public:
        VariableServerVariable( REF2* variableType);
       ~VariableServerVariable();
        const char* getName();
        void stageValue();
        void writeValue( std::ostream& chkpnt_os );

    private:
        VariableServerVariable() {}
        REF2 *varInfo;
        void *address;
        int   size;
        void *stageBuffer;
        bool  deref;
    };
#endif
