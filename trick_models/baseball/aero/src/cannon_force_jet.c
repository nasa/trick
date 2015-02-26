/*********************************** TRICK HEADER **************************
PURPOSE:                     ( Jet fire force )
***************************************************************************/

/*
 * $Log: cannon_force_jet.c,v $
 * Revision 7.1  2006-06-22 14:14:48-05  lin
 * Bump version number for 07
 *
 * Revision 5.2  2004-08-18 10:49:42-05  lin
 * Cleanup cannon_force_jet() In Tutorial
 *
 */

#include "../include/cannon_aero.h"

int cannon_force_jet(
        CANNON_AERO *C )
{
        if ( C->jet_on && C->jet_count < 4 ) {
                C->force_jet[2] = C->force_jet_Z_plus ;
                C->jet_count++ ;
                C->jet_on = 0  ;
        } else {
                C->force_jet[2] = 0.0 ;
        }
        return 0 ;
}
