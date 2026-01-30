
#include <iostream>
#include <string.h>

#include "trick/TrickConstant.hh"
#include "trick/VariableServerSession.hh"

int Trick::VariableServerSession::freeze_init()
{
    if(_enabled && _copy_mode == VS_COPY_SCHEDULED)
    {
        _freeze_next_tics = _cycle_tics;
    }
    else
    {
        _freeze_next_tics = TRICK_MAX_LONG_LONG;
    }
    return 0;
}
