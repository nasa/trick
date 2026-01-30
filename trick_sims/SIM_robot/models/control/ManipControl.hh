#ifndef __MANIPCONTROL_HH_
#define __MANIPCONTROL_HH_
/**************************************************************************
PURPOSE: (2D Manipulator class definitions including kinematics and control)
***************************************************************************/

#include "kinematics/ManipKinemat.hh"
#include "utils/utils.hh"

#include <cmath>
#include <vector>

enum ControlFrame
{
    Task, /* Manual control velocities in the Task Frame */
    EE    /* Manual control velocities in the EE's own frame */
};

class ManipControl
{
public:
    ControlFrame manualFrame; /* -- Frame for Manual Control */

    /* Single Joint Control Mode data */
    int singleJointNum;     /* -- SingleJoint control joint selection */
    double singleJointRate; /* rad/s SingleJoint control joint rate for selected joint */

    /* Manual Control Mode data */
    double manualCommandRate[2]; /* m/s command rate of EE in manual mode */

    /* EEPos Control Mode data */
    double desiredPos[2]; /* m Desired EE position in TaskFrame of EE for EEPos control mode */
    double Kp;            /* -- Proportional gain for EEPos tracking */
    double Kd;            /* -- Derivative gain for EEPos tracking */

    ManipUtils utils; /* -- math utilities */

    double commandedJointRate[2]; /* rad/s calculated required joint rates */

    /* Information required from kinematics */
    double ** jacInv;    /* -- inverse Jacobian as calculated by kinematics */
    double ** R_ee_task; /* -- rot matrix from EE to task frame */

    void manualControl();
    void singleJointControl();
    bool EEPositionAuto(double * pos, double * vel);
    void clearControlCommands();

    ManipControl(int numdof);

private:
    int ndof; /* -- number of degrees of freedom */

    double commandedEERate_Task[2]; /* m/s command rate of EE in Task frame */

    double posEps; /* m  error tolerance for EEPosAuto convergence */

    void calcResolvedJointRates(); /* -- calculate resolved joint rates for EE motion */
};

#endif
