/************************************************************************
PURPOSE: (Simulate a contact.)
LIBRARY DEPENDENCIES:
    ((contact/src/Contact.o))
**************************************************************************/
#ifndef _contact_hh_
#define _contact_hh_
#include "trick/regula_falsi.h"
#include "Ball.hh"

class Contact {

    public:
        Contact(){}
        Ball ** balls;
        unsigned int nballs;
        REGULA_FALSI rf ;

        void ballCollision(Ball &b1, Ball &b2);
        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
        double collision();

};
#endif
