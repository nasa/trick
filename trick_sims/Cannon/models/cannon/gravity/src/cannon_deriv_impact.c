/*****************************************************************************
PURPOSE:    ( Adding contact)
*****************************************************************************/
#include "../include/cannon.h"

int cannon_deriv_impact( CANNON* C )
{
    if ( ! C->impact ) {
        /* Still above ground and flying */
        C->acc[0] = 0.0 ;
        C->acc[1] = -9.81 ;
    }

    return 0 ; 
}
