/************************************************************************
PURPOSE: (Provide a basic simulation for testing GUI)
LIBRARY DEPENDENCIES:
    ((Basic.o))
**************************************************************************/

#include "Basic/include/Basic.hh"

int Basic::default_data() {
    counter = 0;
    return 0;
}

int Basic::state_init() {
    counter = 0;
    return 0;
}

int Basic::state_increment() {
    counter += 1;
    return 0;
}

int Basic::state_integ() {
    return 0;
}