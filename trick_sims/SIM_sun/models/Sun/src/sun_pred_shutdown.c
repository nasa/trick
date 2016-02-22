/*****************************************************************************
PURPOSE: (Shut the thingy down)
*****************************************************************************/
#include <stdio.h>
#include "sun_pred.h"

int sun_pred_shutdown(
      void )
{
     printf("SIM Shutdown.\n");
     fflush(stdout);
     return 0 ;
}
