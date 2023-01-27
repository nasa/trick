/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate links contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Arm.o)
     (Link.o))
*******************************************************************************/
#include <math.h>
#include <iostream>
#include "trick/integrator_c_intf.h"
#include "trick/memorymanager_c_intf.h"
#include "../include/Arm.hh"

int Arm::default_data() {
    links = NULL;
    nlinks = 0;
    return (0);
}

int Arm::state_init() {
    return (0);
}

int Arm::state_deriv() {
    return(0);
}

int Arm::state_integ() {
    int integration_step;
    for (unsigned int ii = 0; ii < nlinks; ii++) {
        load_indexed_state( ii, links[ii]->q);
    }

    for (unsigned int ii = 0; ii < nlinks; ii++) {
        load_indexed_deriv( ii, links[ii]->qdot);
    }

    integration_step = integrate();

    std::cout << "q[] = { ";
    for (unsigned int ii = 0; ii < nlinks; ii++) {
        links[ii]->q = unload_indexed_state( ii );
        links[ii]->q = fmod(links[ii]->q, 2*M_PI);
        std::cout << " " << links[ii]->q;
    }
    std::cout << " }" << std::endl;

    return(integration_step);
}
