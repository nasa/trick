/*************************************************************************
PURPOSE: (Represent Websocket variable server variable.)
LIBRARY DEPENDENCIES:
    ( (../src/WSSessionVariable.o))
**************************************************************************/
#ifndef WSSESSIONVARIABLE_HH
#define WSSESSIONVARIABLE_HH

#include <time.h>
#include <vector>
#include <mongoose.h>
#include <iostream>
#include <trick/reference.h>

#define MAX_ARRAY_LENGTH 4096

class WSsessionVariable {

    public:
        WSsessionVariable( REF2* variableType);
       ~WSsessionVariable();
        const char* getName();
        void stageValue();
        void writeValue( std::ostream& chkpnt_os );

    private:
        WSsessionVariable() {}
        REF2 *varInfo;
        void *address;
        int   size;
        void *stageBuffer;
        bool  deref;
    };
#endif
