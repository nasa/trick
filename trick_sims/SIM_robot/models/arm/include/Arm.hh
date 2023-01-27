/************************************************************************
PURPOSE: (Simulate a arm.)
LIBRARY DEPENDENCIES:
    ((arm/src/Arm.o))
**************************************************************************/
#ifndef _arm_hh_
#define _arm_hh_
#include "Link.hh"

class Arm {

    public:
        Arm(){}
        Link ** links;
        unsigned int nlinks;

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();

};
#endif
