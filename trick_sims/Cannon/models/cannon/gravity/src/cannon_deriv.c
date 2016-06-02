/*****************************************************************************
PURPOSE:    ( Try Trick integration )
*****************************************************************************/
#include "../include/cannon.h"
#include "../include/cannon_integ_proto.h"

int cannon_deriv(
      CANNON* C )
{
        C->acc[0] = 0.0 ;
        C->acc[1] = -9.81 ;

        return 0 ; 
}
