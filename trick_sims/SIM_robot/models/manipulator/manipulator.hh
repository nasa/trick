#ifndef __MANIPULATOR_HH_
#define __MANIPULATOR_HH_
/**************************************************************************
PURPOSE: (2D Manipulator class definitions including kinematics and control)
***************************************************************************/

#include "control/ManipControl.hh"
#include "kinematics/ManipKinemat.hh"
#include "trick/integrator_c_intf.h"
#include "utils/utils.hh"

#include <cmath>
#include <iostream>
#include <new>

enum ControlMode
{
    Nope,        /* No control, just chill */
    Cancel,      /* Cancel current mode, clear control vars, and set to Nope */
    SingleJoint, /* Control a single joint drive direction */
    Manual,      /* Inv Kin resolved-rate control of end effector rate */
    EEPos        /* Drive EE to desired location */
};

class PlanarManip
{
public:
    int ndof;         /* -- number of degrees of freedom */
    ControlMode mode; /* -- Control mode to use */

    ManipKinemat kinemat;    /* -- class the calculates for/inv kinematics */
    ManipControl controller; /* -- controller class */

    PlanarManip(int numDof);

    void calcKinematics(); /* -- call kinematics routines */
    void control();        /* -- call control routines */
    int stateDeriv();      /* -- update velocities for integration */
    int updateState();     /* -- update manipulator state */
};

#endif
