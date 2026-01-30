/*****************************************************************************
PURPOSE: (Shut the thingy down)
*****************************************************************************/
#include "sun_pred.h"
#include <stdio.h>

int sun_pred_shutdown(void)
{
    printf("SIM Shutdown.\n");
    fflush(stdout);
    return 0;
}
