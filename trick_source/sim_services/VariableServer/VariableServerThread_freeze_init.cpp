
#include <iostream>
#include <string.h>

#include "trick/VariableServerThread.hh"
#include "trick/TrickConstant.hh"

int Trick::VariableServerThread::freeze_init() {
    if ( enabled and copy_mode == VS_COPY_SCHEDULED) {
        freeze_next_tics = cycle_tics ;
    } else {
        freeze_next_tics = TRICK_MAX_LONG_LONG ;
    }
    return 0 ;
}

