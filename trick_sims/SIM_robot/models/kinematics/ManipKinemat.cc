/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate a planar, 2DOF manipulator for Trick example sim.)
LIBRARY DEPENDENCY:
    ((kinematics/ManipKinemat.o)
     (utils/utils.o)
    )
PROGRAMMERS:
    (((Sean Harmeyer) (NASA) (Dec 2022) (Trick Example Sim)))
*******************************************************************************/

#include "kinematics/ManipKinemat.hh"
#include "include/trick/memorymanager_c_intf.h"

ManipKinemat::ManipKinemat(int numDof):ndof(numDof)
{

    int i,j;

    /* These vectors and matrices don't explicitly depend on the 
     * number of degrees of freedom */
    V_ee          = (double *)TMM_declare_var_1d("double",2);
    P_task_base   = (double *)TMM_declare_var_1d("double",2);
    P_joint_ee    = (double *)TMM_declare_var_1d("double",2);
    P_task_ee     = (double *)TMM_declare_var_1d("double",2);
    joint_ee      = (double *)TMM_declare_var_1d("double",2);

    R_task_base   = (double **)TMM_declare_var_1d("double*",2);
    R_base_task   = (double **)TMM_declare_var_1d("double*",2);
    R_ee_joint    = (double **)TMM_declare_var_1d("double*",2);
    jacobian      = (double **)TMM_declare_var_1d("double*",2);
    R_ee_task     = (double **)TMM_declare_var_1d("double*",2);

    /* total number of degrees of freedom determines the size of all of these 
     * vectors and matrices, so we size them accordingly */

    joint_u       = (double *)TMM_declare_var_1d("double",ndof);
    joint_q       = (double *)TMM_declare_var_1d("double",ndof);
    joint_w       = (double *)TMM_declare_var_1d("double",ndof);

    joint_l       = (double **)TMM_declare_var_1d("double*",ndof);
    jacInv        = (double **)TMM_declare_var_1d("double*",ndof);

    P_joint_joint = (double **)TMM_declare_var_1d("double*",ndof);
    P_task_joint  = (double **)TMM_declare_var_1d("double*",ndof);
    V_joint_joint = (double **)TMM_declare_var_1d("double*",ndof);
    V_joint_task  = (double **)TMM_declare_var_1d("double*",ndof);

    R_joint       = (double ***)TMM_declare_var_1d("double**",ndof);
    R_joint_task  = (double ***)TMM_declare_var_1d("double**",ndof);

    for(i=0;i<2;i++)
    {
        R_task_base[i] = (double *)TMM_declare_var_1d("double",2);
        R_base_task[i] = (double *)TMM_declare_var_1d("double",2);
        R_ee_task[i] = (double *)TMM_declare_var_1d("double",2);
        R_ee_joint[i] = (double *)TMM_declare_var_1d("double",2);
        jacobian[i] = (double *)TMM_declare_var_1d("double",ndof);
    }


    for(i=0;i<ndof;i++)
    {

        joint_l[i] = (double *)TMM_declare_var_1d("double",2);
        P_joint_joint[i] = (double *)TMM_declare_var_1d("double",2);
        P_task_joint[i] = (double *)TMM_declare_var_1d("double",2);
        V_joint_joint[i] = (double *)TMM_declare_var_1d("double",2);
        V_joint_task[i] = (double *)TMM_declare_var_1d("double",2);

        R_joint[i] = (double **)TMM_declare_var_1d("double*",2);
        R_joint_task[i] = (double **)TMM_declare_var_1d("double*",2);
        jacInv[i]    = (double *)TMM_declare_var_1d("double",2);

        for(j=0;j<ndof;j++)
        {
            R_joint[i][j] = (double *)TMM_declare_var_1d("double",2);
            R_joint_task[i][j] = (double *)TMM_declare_var_1d("double",2);
        }

    }
 
    /* base relative to task in task frame*/
    P_task_base[0] = 0;
    P_task_base[1] = 0;

    R_base_task[0][0] = 1;
    R_base_task[0][1] = 0;
    R_base_task[1][0] = 0;
    R_base_task[1][1] = 1;

    R_task_base[0][0] = 1;
    R_task_base[0][1] = 0;
    R_task_base[1][0] = 0;
    R_task_base[1][1] = 1;


    /* initial joint angles */
    joint_q[0] = 0;
    joint_q[1] = 0;

    /* link lengths */
    joint_l[0][0] = 2;
    joint_l[0][1] = 0;
    joint_l[1][0] = 2;
    joint_l[1][1] = 0;

    /* EE offset */
    joint_ee[0] = 0;
    joint_ee[1] = 0;

    /* Rotation from EE to previous joint */
    R_ee_joint[0][0] = 1;
    R_ee_joint[0][1] = 0;
    R_ee_joint[1][0] = 0;
    R_ee_joint[1][1] = 1;
    
    jacobian[0][0] = 0;
    jacobian[0][1] = 0;
    jacobian[1][0] = 0;
    jacobian[1][1] = 0;

    jacDet         = 0;

    singularityTolerance = .17453;
    checkSingularities   = true;

}

void ManipKinemat::forwardKinPos()
{
    int i;      // loop counter
    double c,s; // sine and cosine shorthand

    // rescale angles to 0 < q < 2*PI
    for(i=0; i<ndof; i++)
    {
        if (joint_q[i] > 2.0 * M_PI)
            joint_q[i] -= 2.0 * M_PI;
        else if (joint_q[i] < -2.0 * M_PI)
            joint_q[i] += 2.0 * M_PI;
    }

    // Calculate current rotation matrices for all frames
    for (i=0; i<ndof; i++)
    {
        c = cos(joint_q[i]);
        s = sin(joint_q[i]);

        R_joint[i][0][0]=  c;
        R_joint[i][0][1]= -s;
        R_joint[i][1][0]=  s;
        R_joint[i][1][1]=  c;

        // Calculate position of every joint tip in previous frame
        utils.MVMult(P_joint_joint[i], (double** )R_joint[i], joint_l[i]);
    }


    // Calculate R from every joint back to task
    utils.Transpose(R_base_task, R_task_base);

    for(i=0;i<ndof;i++)
    {
        if(i == 0)
            utils.MMMult(R_joint_task[i], R_base_task, R_joint[i]);
        else
            utils.MMMult(R_joint_task[i], R_joint_task[i-1], R_joint[i]);
    }

    // Calculate R from EE to task
    utils.MMMult(R_ee_task, R_joint_task[ndof-1], R_ee_joint);



    // Calculate vector from task to each joint in task
    for(i=0;i<ndof;i++)
    {
        if (i==0)
            utils.MVMult(P_task_joint[i],R_joint_task[i],joint_l[i]);
        else
        {
            utils.MVMult(P_task_joint[i],R_joint_task[i],joint_l[i]);
            utils.VAdd(P_task_joint[i],P_task_joint[i],P_task_joint[i-1]);
        }
    }

    // Calculate vector from task to EE in task
    
    utils.MVMult(P_task_ee,R_ee_task,joint_ee);
    utils.VAdd(P_task_ee,P_task_ee,P_task_joint[ndof-1]);


}


void ManipKinemat::forwardKinVel()
{

    /* like the jacobian, these are hand-calculated for a 2-dof planar.  
     * Anything more would require velocity propagation to determine the
     * velocity of the end-effector and intermediate points */

    double l1, l2, s1, c1, s12, c12;
    l1  = joint_l[0][0];
    l2  = joint_l[1][0];

    s1  = sin(joint_q[0]);
    s12 = sin(joint_q[0]+joint_q[1]);

    c1  = cos(joint_q[0]);
    c12 = cos(joint_q[0]+joint_q[1]);


    V_ee[0] = ( -l1*s1 - l2*s12 )*joint_w[0] - l2*s12*joint_w[1];
    V_ee[1] = ( l1*c1 + l2*c12 )*joint_w[0] + l2*c12*joint_w[1];



}


void ManipKinemat::calcJacobian()
{

    /* These are hand-calculated due to the simplicity of the manipulator 
     * system.  The algorithmic method of determining the elements of 
     * the jacobian, while efficient and generic, are difficult to read in
     * code and are beyond the scope of a simple robotic tutorial
     */

    double s1,c1,s12,c12;
    s1  = sin(joint_q[0]);
    s12 = sin(joint_q[0]+joint_q[1]); 
    c1  = cos(joint_q[0]);
    c12 = cos(joint_q[0]+joint_q[1]); 

    double l1, l2;
    l1 = joint_l[0][0];
    l2 = joint_l[1][0];

    jacobian[0][0] = -l1*s1 - l2*s12;
    jacobian[0][1] = -l2*s12;
    jacobian[1][0] = l1*c1 + l2*c12;
    jacobian[1][1] = l2*c12;


    /* Calculate the inverse jacobian as well */

    jacDet = jacobian[0][0]*jacobian[1][1] - jacobian[0][1]*jacobian[1][0];

    jacInv[0][0] =  jacobian[1][1]/jacDet;
    jacInv[0][1] = -jacobian[0][1]/jacDet;
    jacInv[1][0] = -jacobian[1][0]/jacDet;
    jacInv[1][1] =  jacobian[0][0]/jacDet;

}

bool ManipKinemat::isSingular()
{
    if( (fabs(joint_q[1]) < singularityTolerance) || ( fabs( fabs(joint_q[1]) - M_PI)  < singularityTolerance ) )
        return true;
    else
        return false;
}
