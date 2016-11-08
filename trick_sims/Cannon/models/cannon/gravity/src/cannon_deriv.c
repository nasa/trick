/*****************************************************************************
PURPOSE:    ( Try Trick integration )
*****************************************************************************/
#include "../include/cannon.h"

int cannon_deriv( CANNON* C ) {

      if (!C->impact) {
          C->acc[0] =  0.00 ;
          C->acc[1] = -9.81 ;
          C->timeRate = 1.00;
      }
      return 0 ;
}
