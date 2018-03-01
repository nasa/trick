/************************TRICK HEADER*************************
  PURPOSE: ( Trick integration )
PROGRAMMERS:
    (((Scott P. Fennell) (CACI International Inc.) (January 2018) (Trick Learning Project)))
*************************************************************/
#include "../include/msd_numeric.hh"
#include "../include/msd.hh"
#include <math.h>

int MSD::state_deriv(void) {
    /* system equation for second derivitive */
    a = (F - b*v - k*x)/m;
    
    return(0);
}