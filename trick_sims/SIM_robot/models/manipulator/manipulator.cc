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

PlanarManip::PlanarManip(int numDof)
    : ndof(numDof),
      kinemat(numDof),
      controller(numDof)
{
    int i = 0;
    /* just throw some pointers around */

    controller.R_ee_task = kinemat.R_ee_task;
    controller.jacInv = kinemat.jacInv;
    for(i = 0; i < ndof; i++)
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

            if(controller.singleJointNum < 0 || controller.singleJointNum > ndof)
            {
                controller.singleJointNum = 0;
            }

            controller.singleJointControl();
            break;

        case Manual:
            if(kinemat.checkSingularities && kinemat.isSingular())
            {
                std::cout << "Singularity approached, EE control disabled." << std::endl;
                mode = Nope;
                controller.clearControlCommands();
                return;
            }

            controller.manualControl();
            break;

        case EEPos:
            if(kinemat.checkSingularities)
            {
                if(!kinemat.isSingular())
                {
                    positionReached = controller.EEPositionAuto(&(kinemat.P_task_ee[0]), &(kinemat.V_ee[0]));
                }
                else
                {
                    std::cout << "Singularity approached, EE control disabled." << std::endl;
                    mode = Nope;
                    controller.clearControlCommands();
                    return;
                }
            }
            else
            {
                positionReached = controller.EEPositionAuto(&(kinemat.P_task_ee[0]), &(kinemat.V_ee[0]));
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

int PlanarManip::stateDeriv()
{
    return (0);
}

int PlanarManip::updateState()
{
    int integration_step, i;

    for(i = 0; i < ndof; i++)
    {
        /* This is a kinematic sim with perfect control, so the commanded joint rates
         * magically become the real joint rates */
        kinemat.joint_w[i] = controller.commandedJointRate[i];
    }

    for(i = 0; i < ndof; i++)
    {
        load_indexed_state(i, kinemat.joint_q[i]);
    }
    for(i = 0; i < ndof; i++)
    {
        load_indexed_deriv(i, kinemat.joint_w[i]);
    }

    integration_step = integrate();

    for(i = 0; i < ndof; i++)
    {
        kinemat.joint_q[i] = unload_indexed_state(i);
        kinemat.joint_q[i] = fmod(kinemat.joint_q[i], 2 * M_PI);
    }

    //    std::cout << "q = [ " << kinemat.joint_q[0] << kinemat.joint_q[1] << " ]" << std::endl;

    return (integration_step);
}
