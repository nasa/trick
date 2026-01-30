/************************************************************************
PURPOSE: (Shutdown the simulation)
*************************************************************************/
#include "../include/body.hh"
#include "trick/exec_proto.h"

int BODY::body_shutdown()
{
    delete[] mat_mass_dyn;
    delete[] mat_L;

    return 0;
}
