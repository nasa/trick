/************************************************************************
PURPOSE: (Provide a basic simulation for testing GUI)
LIBRARY DEPENDENCIES:
    ((Basic/src/Basic.o))
**************************************************************************/

#ifndef BASIC_HH
#define BASIC_HH

class Basic {
    public:
    int counter;    /* -- Just a counter variable */
    
    int default_data();
    int state_init();
    int state_increment();
    int state_integ();
};

#endif