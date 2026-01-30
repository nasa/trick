/********************************* TRICK HEADER *******************************
PURPOSE: ( Control algorithms for 2-dof planar manip.)
LIBRARY DEPENDENCY:
    ((control/ManipControl.o)
     (utils/utils.o)
    )
PROGRAMMERS:
    (((Sean Harmeyer) (NASA) (Dec 2022) (Trick Example Sim)))
*******************************************************************************/

#include "control/ManipControl.hh"
#include <algorithm>
#include <cmath>

ManipControl::ManipControl(int numDof)
    : ndof(numDof)
{
    manualFrame = Task;
    commandedJointRate[0] = 0.0;
    commandedJointRate[1] = 0.0;
    singleJointNum = 0;
    singleJointRate = 0;
    desiredPos[0] = 2;
    desiredPos[1] = 0;

    Kp = 2.0;
    Kd = 0.3;

    posEps = .01;
}

void ManipControl::manualControl()
{
    int i = 0;

    /* command_ee_rate should be input from the user and will be converted to
     * motion in the Task Frame */
    if(manualFrame == EE)
    {
        /* In the EE control frame, the commanded manual rate needs to be rotated
         * from the EE frame to the Task frame */
        utils.MVMult(commandedEERate_Task, R_ee_task, manualCommandRate);
    }
    else
    {
        /* In the Task control frame, the commanded manual rate is already in the Task
         * Frame, so no rotation required */
        for(i = 0; i < ndof; i++)
        {
            commandedEERate_Task[i] = manualCommandRate[i];
        }
    }

    calcResolvedJointRates();
}

void ManipControl::calcResolvedJointRates()
{
    utils.MVMult(commandedJointRate, jacInv, commandedEERate_Task);
}

void ManipControl::singleJointControl()
{
    /* Drive the selected joint at the commanded rate */
    commandedJointRate[singleJointNum] = singleJointRate;
}

bool ManipControl::EEPositionAuto(double * curPos, double * curVel)
{
    /* Returns true when desired EE position has been reached, false otherwise */

    double posErr[2];
    int i;
    bool withinTol[ndof];
    bool posReached = true;

    for(i = 0; i < ndof; i++)
    {
        withinTol[i] = false;
        posErr[i] = desiredPos[i] - curPos[i];
        if(fabs(posErr[i]) <= posEps)
        {
            withinTol[i] = true;
        }

        commandedEERate_Task[i] = Kp * posErr[i] - Kd * curVel[i];
    }

    for(i = 0; i < ndof; i++)
    {
        if(!withinTol[i])
        {
            calcResolvedJointRates();
            posReached = false;
            break;
        }
    }

    return (posReached);
}

void ManipControl::clearControlCommands()
{
    singleJointNum = 0;
    singleJointRate = 0;

    int i = 0;

    for(i = 0; i < ndof; i++)
    {
        manualCommandRate[i] = 0;
        desiredPos[i] = 0;
        commandedEERate_Task[i] = 0;
        commandedJointRate[i] = 0;
    }
}
