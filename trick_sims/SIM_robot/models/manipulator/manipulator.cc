/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate a planar, 2DOF manipulator for Trick example sim.)
LIBRARY DEPENDENCY:
    (
     (manipulator/manipulator.o)
     (kinematics/ManipKinemat.o)
     (control/ManipControl.o)
     (utils/utils.o)
    )
PROGRAMMERS:
    (((Sean Harmeyer) (NASA) (Dec 2022) (Trick Example Sim)))
*******************************************************************************/

#include "manipulator/manipulator.hh"

PlanarManip::PlanarManip(int numDof) : kinemat(numDof), controller(numDof), ndof(numDof)
{

    int i = 0;
    /* just throw some pointers around */

    controller.R_ee_task = kinemat.R_ee_task;
    controller.jacInv    = kinemat.jacInv;
    for(i=0;i<ndof;i++)
    {
        controller.jacInv[i] = kinemat.jacInv[i];
    }

}


void PlanarManip::calcKinematics()
{

    kinemat.forwardKinPos();
    kinemat.forwardKinVel();
    kinemat.calcJacobian();

}

void PlanarManip::control()
{

    bool positionReached = false;
    switch(mode)
    {
        /* No need to check singularities in SingleJoint mode, because they only arise with inverse 
         * Jacobians and there's no need to use an inverse Jacobian here */
        case SingleJoint:

            if (controller.singleJointNum < 0 || controller.singleJointNum > ndof)
                controller.singleJointNum = 0;

            controller.singleJointControl();
            break;

        case Manual:
            if ( kinemat.checkSingularities )
            {
                if ( kinemat.isSingular() )
                {
                    std::cout<<"Singularity approached, EE control disabled."<<std::endl;
                    mode = Nope;
                    controller.clearControlCommands();
                    return;
                }

            }
            
            controller.manualControl();
            break;

        case EEPos:
            positionReached = controller.EEPositionAuto( &(kinemat.P_task_ee[0]), &(kinemat.V_ee[0]) );
            if(positionReached)
            {
                std::cout<<"EE Position Reached!"<<std::endl;
                mode = Nope;
                controller.clearControlCommands();
            }
            break;

        case Cancel:
            controller.clearControlCommands();
            mode = Nope;
            break;
        default:
        case Nope:
            break;

    }

}

void PlanarManip::updateState()
{

    int i;
    double dt = exec_get_job_cycle("");

    for(i=0;i<ndof;i++)
    {
        /* This is a kinematic sim with perfect control, so the commanded joint rates 
         * magically become the real joint rates */
        kinemat.joint_w[i] = controller.commandedJointRate[i];

        /* Ideal commanded motion, angles update perfectly */
        kinemat.joint_q[i] += kinemat.joint_w[i]*dt;
    }
    return;

}


