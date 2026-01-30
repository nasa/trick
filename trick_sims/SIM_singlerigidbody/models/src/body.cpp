/**************************************************************************
PURPOSE: (Test trick macros (matrix and vector) as well as trick functions
      including euler_matrix, deuler_123, and dLU_Choleski. Perform a
      single body motion with six degrees of freedom)
LIBRARY DEPENDENCIES:
    ((src/body.o))
**************************************************************************/

#include <math.h>

#include "trick/integrator_c_intf.h"
#include "trick/matrix_macros.h"
#include "trick/reference_frame.h"
#include "trick/trick_math_error.h"
#include "trick/trick_math_proto.h"
#include "trick/vector_macros.h"

#include "../include/body.hh"

BODY::BODY() {}

BODY::~BODY() {}

/*************************************************************************
FUNCTION: SingleRigidBody::default_data()
PURPOSE:` (Creates an object)
*************************************************************************/

void BODY::default_data()
{
    // Initializing vectors and matricies to zero.

    for(int i = 0; i < 6; i++)
    {
        vec_a[i] = 0.0;
    }

    for(int i = 0; i < 6; i++)
    {
        vec_b[i] = 0.0;
    }

    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            mat_mass[i][j] = 0.0;
        }
    }
    V_INIT(pos);
    V_INIT(vel);
    V_INIT(acc);
    V_INIT(force);
    V_INIT(force_remain);
    V_INIT(torque);
    V_INIT(torque_remain);
    V_INIT(angle);
    V_INIT(omega);
    V_INIT(omegaDot);
    V_INIT(angle_force);

    M_INIT(CM_skew);
    M_INIT(m_CM_skew);
    M_INIT(massmatrix);
    M_INIT(rotate);

    // Inputs
    mass = 20.0;
    radius = 2.0;

    POS_INIT[0] = 0.0;
    POS_INIT[1] = 0.0;
    POS_INIT[2] = 0.0;

    ANGLE_INIT[0] = 0.0;
    ANGLE_INIT[1] = 0.0;
    ANGLE_INIT[2] = 0.0;

    OMEGA_INIT[0] = 0.0;
    OMEGA_INIT[1] = 0.0;
    OMEGA_INIT[2] = 0.0;

    FORCE_INIT[0] = 0.0;
    FORCE_INIT[1] = 0.0;
    FORCE_INIT[2] = 10.0;

    VEL_INIT[0] = 0.0;
    VEL_INIT[1] = 0.0;
    VEL_INIT[2] = 0.0;

    POS_FORCE[0] = 2.0;
    POS_FORCE[1] = 0.0;
    POS_FORCE[2] = 0.0;
}

/************************************************************************
FUNCTION: SingleRigidBody::init()
PURPOSE: (Initializing state variables, creating inertia and CoM skew
      matrices)
************************************************************************/

void BODY::init()
{
    V_COPY(pos, POS_INIT);
    V_COPY(vel, VEL_INIT);
    V_COPY(angle, ANGLE_INIT);
    V_COPY(omega, OMEGA_INIT);

    // Center of mass skew matrix
    V_INIT(CM);

    V_SKEW(CM_skew, CM);

    MxSCALAR(m_CM_skew, CM_skew, mass);
    MxSCALAR(neg_m_CM_skew, m_CM_skew, -1.0);

    // Creates 3x3 matrix for inerta
    inertia = (2.0 / 5.0) * mass * radius * radius;
    inertia_matrix[0][0] = inertia_matrix[1][1] = inertia_matrix[2][2] = inertia;

    // Creates 3x3 matrix for mass
    massmatrix[0][0] = massmatrix[1][1] = massmatrix[2][2] = mass;

    // Combines inertia, center of mass, and mass matrix into a 6x6 matrix
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            mat_mass[i][j] = massmatrix[i][j];
        }
    }

    for(int i = 0; i < 3; i++)
    {
        for(int j = 3; j < 6; j++)
        {
            mat_mass[i][j] = neg_m_CM_skew[i][j - 3];
        }
    }

    for(int i = 3; i < 6; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            mat_mass[i][j] = m_CM_skew[i - 3][j];
        }
    }

    for(int i = 3; i < 6; i++)
    {
        for(int j = 3; j < 6; j++)
        {
            mat_mass[i][j] = inertia_matrix[i - 3][j - 3];
        }
    }

    // Temporary vector and dynamic memory for Choleski
    for(int i = 0; i < 6; i++)
    {
        tmp_vec[i] = 0.0;
    }

    mat_L = new double *[6];
    mat_mass_dyn = new double *[6];
}

/*****************************************************************************
FUNCTION: BODY::rotation_matrix()
PURPOSE: (Calculates transformation matrix from provided angles)
*****************************************************************************/
void BODY::rotation_matrix()
{
    // euler_matrix outputs the rotate matrix from angle
    euler_matrix(angle, rotate, 0, Roll_Pitch_Yaw);
}

/******************************************************************************
FUNCTION: BODY::calcforce()
PURPOSE: (Calculates force wrt inertial frame)
******************************************************************************/
void BODY::calcforce()
{
    MxV(force, rotate, FORCE_INIT);
}

/*****************************************************************************
FUNCTION: BODY::calctorque()
PURPOSE: (Calculates torque wrt inertial frame)
*****************************************************************************/
void BODY::calctorque()
{
    double torque_force[3];

    V_CROSS(torque_force, POS_FORCE, FORCE_INIT);
    MxV(torque, rotate, torque_force);
}

/****************************************************************************
FUNCTION: BODY::calcforce_remain()
PURPOSE: (Calculate remaining force wrt inertial frame)
****************************************************************************/
void BODY::calcforce_remain()
{
    double cross1[3];
    double cross2[3];

    V_CROSS(cross1, omega, CM);
    V_CROSS(cross2, omega, cross1);
    V_SCALE(force_remain, cross2, mass);
}

/***************************************************************************
FUNCTION: BODY::calctorque_remain()
PURPOSE: (Calculate remaining torque wrt inertial frame)
***************************************************************************/
void BODY::calctorque_remain()
{
    double I_w[3];

    MxV(I_w, inertia_matrix, omega);
    V_CROSS(torque_remain, omega, I_w);
}

/***************************************************************************
FUNCTION: BODY::eq_setup()
PURPOSE: (Establishes b column vector for final eq M*a=b)
***************************************************************************/
void BODY::eq_setup()
{
    double remainder_force[3];
    double remainder_torque[3];

    // Subtracts remaining from force & torque to get vec_b.
    V_SUB(remainder_force, force, force_remain);
    V_SUB(remainder_torque, torque, torque_remain);

    for(int i = 0; i < 3; i++)
    {
        vec_b[i] = remainder_force[i];
    }

    for(int i = 3; i < 6; i++)
    {
        vec_b[i] = remainder_torque[i - 3];
    }
}

/**************************************************************************
FUNCTION: BODY::eq_solver()
PURPOSE: (Performs choleski decomposition and computes linear and angular
          accelerations)
**************************************************************************/

void BODY::eq_solver()
{
    // Solving a = b * M^-1

    for(int i = 0; i < 6; i++)
    {
        mat_mass_dyn[i] = mat_mass[i];
        mat_L[i] = new double[6];
    }

    dLU_Choleski(mat_mass_dyn, mat_L, tmp_vec, 6, vec_b, vec_a, 0);

    // Acceleration output
    for(int i = 0; i < 3; i++)
    {
        acc[i] = vec_a[i];
    }

    // Angular accleration output
    for(int i = 0; i < 3; i++)
    {
        omegaDot[i] = vec_a[i + 3];
    }
}

/***************************************************************************
FUNCTION: BODY::derivative()
PURPOSE: (Calls all functions in desired order for calculations)
***************************************************************************/
void BODY::derivative()
{
    rotation_matrix();
    calcforce();
    calctorque();
    calcforce_remain();
    calctorque_remain();
    eq_setup();
    eq_solver();
}

/***************************************************************************
FUNCTION: BODY::integrate()
PURPOSE: (Sets up trick integration)
***************************************************************************/

int BODY::integ()
{
    int integration_step;

    load_state(&pos[0],
               &pos[1],
               &pos[2],
               &vel[0],
               &vel[1],
               &vel[2],
               &angle[0],
               &angle[1],
               &angle[2],
               &omega[0],
               &omega[1],
               &omega[2],

               NULL);

    load_deriv(&vel[0],
               &vel[1],
               &vel[2],
               &acc[0],
               &acc[1],
               &acc[2],
               &omega[0],
               &omega[1],
               &omega[2],
               &omegaDot[0],
               &omegaDot[1],
               &omegaDot[2],

               NULL);

    integration_step = integrate();

    unload_state(&pos[0],
                 &pos[1],
                 &pos[2],
                 &vel[0],
                 &vel[1],
                 &vel[2],
                 &angle[0],
                 &angle[1],
                 &angle[2],
                 &omega[0],
                 &omega[1],
                 &omega[2],

                 NULL);

    return (integration_step);
}
