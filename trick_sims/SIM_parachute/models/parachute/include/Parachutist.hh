/*************************************************************************
PURPOSE: (Parachutist Structure)
LIBRARY DEPENDENCIES:
    (
     (parachutist/src/Parachutist.o)
    )

**************************************************************************/

#ifndef _parachutist_hh_
#define _parachutist_hh_
#include "sim_services/Integrator/include/regula_falsi.h"

class Parachutist {

    public:
        double altitude ;                    /* m     xyz-position */
        double velocity ;                    /* m/s   xyz-velocity */
        double acceleration ;                /* m/s2  xyz-acceleration  */

        double area;                         /* m2 */
        double Cd;                           /* -- */
        double mass;                         /* kg */

        bool touchDown;                      /* -- */

        REGULA_FALSI rf ;

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
        double touch_down(double groundAltitude);
};

#endif
