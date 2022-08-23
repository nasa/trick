
#include <iostream>
#include <string.h>

#include "trick/VariableServerSession.hh"
#include "trick/TrickConstant.hh"

int Trick::VariableServerSession::freeze_init() {
    if ( enabled && copy_mode == VS_COPY_SCHEDULED) {
        freeze_next_tics = cycle_tics ;
    } else {
        freeze_next_tics = TRICK_MAX_LONG_LONG ;
    }
    return 0 ;
}

