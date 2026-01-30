/*********************************************************************
PURPOSE: (Simulate a single rigid body with six degrees of freedom)
LIBRARY DEPENDENCIES:
    ((SingleRigidBody/src/body.o))
*********************************************************************/
#ifndef BODY_HH
#define BODY_HH

class BODY
{
public:
    BODY();
    ~BODY();
    // Variables

    // Initializes state variables
    double POS_INIT[3];
    double VEL_INIT[3];
    double ANGLE_INIT[3];
    double OMEGA_INIT[3];

    // State variables
    double pos[3];
    double vel[3];
    double angle[3];
    double omega[3];

    double acc[3];
    double omegaDot[3];

    double force[3];
    double FORCE_INIT[3];
    double force_remain[3];
    double POS_FORCE[3];

    double torque[3];
    double TORQUE_INIT[3];
    double torque_remain[3];

    double inertia;
    double inertia_matrix[3][3];

    double radius;
    double mass;
    double mat_mass[6][6];
    double massmatrix[3][3];

    double CM[3];
    double CM_skew[3][3];
    double m_CM_skew[3][3];
    double neg_m_CM_skew[3][3];

    double vec_a[6];
    double vec_b[6];

    double rotate[3][3];
    double angle_force[3];

    double tmp_vec[6];
    double ** mat_mass_dyn;
    double ** mat_L;

    // Methods
    void default_data();
    void init();
    void derivative();
    int integ();
    int body_shutdown();

    void rotation_matrix();
    void calcforce();
    void calctorque();
    void calcforce_remain();
    void calctorque_remain();
    void eq_setup();
    void eq_solver();
};

#endif
