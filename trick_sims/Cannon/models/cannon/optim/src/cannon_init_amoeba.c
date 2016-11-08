/*********************************** TRICK HEADER **************************
PURPOSE: ( Init the Amoeba )
LIBRARY DEPENDENCY: ((amoeba.o))
PROGRAMMERS: (Keith)
***************************************************************************/
#include <stdio.h>
#include "cannon/aero/include/cannon_aero.h"
#include "../include/amoeba.h"
#include "../include/amoeba_proto.h"

int cannon_init_amoeba(
      AMOEBA* A  )
{
        amoeba_init( A, 4, 1.0e-3, 100, NULL, 0.0 ) ;
        return ( 0 );
}
