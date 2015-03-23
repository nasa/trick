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
        double position ;                    /* m     xyz-position */
        double velocity ;                    /* m/s   xyz-velocity */
        double acceleration ;                /* m/s2  xyz-acceleration  */

        double crossSectionalArea;           /* m2 */
        double crossSectionalAreaRate;       /* m2/s */
        double cooefficientOfDrag;           /* -- */
        double cooefficientOfDragRate;       /* -- */
        double mass;                         /* kg */

        double parachuteDeploymentStartTime; /* s */
        double parachuteDeploymentDuration;  /* s */

        static const double crossSectionalArea_freefall;  /* m2 */
        static const double crossSectionalArea_parachute; /* m2 */
        static const double cooefficientOfDrag_freefall;  /* -- */
        static const double cooefficientOfDrag_parachute; /* -- */

        bool touchDown;                      /* -- */

        REGULA_FALSI rf ;

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
        int parachute_control();
        double touch_down();
    private:
        double parachuteDeploymentEndTime;   /* s */
};

#endif
