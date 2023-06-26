#ifndef __MANIPKINEMAT_HH_
#define __MANIPKINEMAT_HH_
/**************************************************************************
PURPOSE: (2D Manipulator kinematics)
LIBRARY DEPENDENCY: ( (kinematics/ManipKinemat.cc) )
***************************************************************************/
#include <cmath>
#include "include/trick/exec_proto.h"
#include "utils/utils.hh"
#include <iostream>
class ManipKinemat
{
    public:


        double *joint_u;              /* --    axis of rotation for each joint */
        double *joint_q;              /* rad   joint angle of each joint */
        double *joint_w;              /* rad/s joint rate of each joint */

        double **joint_l;           /* m  link lengths of each joint */
        double *joint_ee;             /* m  length from last joint to ee */


        double *P_task_base;          /* m  vector task->base expressed in task */
        double **R_task_base;       /* -- rotation matrix task->base */
        double **R_base_task;           /* -- rotation matrix base->task */

        //(first index is base->joint_0 expressed in base)
        double **P_joint_joint;     /* m  link_i+1->link_i in frame i */ 
        
        double *P_joint_ee;           /* m  link_n-1 -> ee in frame n-1 */
        double **P_task_joint;      /* m  task->link_i in task frame */
        double *P_task_ee;            /* m  task->ee in task frame */



        // first index is joint_0->base rotation
        double ***R_joint;        /* -- rotation matrix frames i+1->i */

        double ***R_joint_task;   /* -- rotation matrix frame i+1-task */
        double **R_ee_joint;        /* -- rotation matrix frame ee->n */

        double **R_ee_task;             /* -- rotation matrix frame ee->task */


        double **V_joint_joint;     /* m/s velocity of frame i in i-1 */
        double *V_ee;                 /* m/s vel of EE in task */
        double **V_joint_task;      /* m/s vel of joint i in task frame */

        double **jacobian;          /* -- transform from angular to linear vel */
        double **jacInv;                /* -- Inverse Jacobian */
        double jacDet;                  /* -- Determinant of the Jacobian */


        bool   checkSingularities;      /* --     whether to check for kinematic singularities  */
        double singularityTolerance;    /* rad tolerance for singularity approach */

        ManipUtils      utils;

        ManipKinemat(int numDof);

        void forwardKinPos();
        void forwardKinVel();
        void calcJacobian();
        bool isSingular();

    private:

        int    ndof;                    /* --    number of degrees of freedom */
};

#endif
