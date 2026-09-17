/*********************************************************************
PURPOSE: (Simulate a double compound pendulum)
LIBRARY DEPENDENCIES: 
	((models/src/pendulum.o))
*********************************************************************/
#ifndef PENDULUM_HH
#define PENDULUM_HH

class PENDULUM {
public: 
	PENDULUM();
	~PENDULUM();
	//Variables
	
	//Initializes state variables
	double POS1_INIT[2];					
	double POS2_INIT[2];
	double VEL1_INIT[2];
	double VEL2_INIT[2];
	double ANGLE1_INIT;
	double ANGLE2_INIT;
	double OMEGA1_INIT;
	double OMEGA2_INIT;
	double OMEGADOT1_INIT;
	double OMEGADOT2_INIT;

	//State variables
	double pos1[2];					/* m position of center of mass wrt inertial frame */
	double pos2[2];					/* m position of center of mass wrt inertial frame */
	double vel1[2];					/* m/s velocity wrt inertial fram */
	double vel2[2];					/* m/s velocity wrt inertial fram */
	double angle1;					/* rad angle wrt inertial frame */
	double angle2;					/* rad angle wrt inertial frame */
	double omega1;					/* rad/s angular velocity wrt inertial frame */
	double omega2;					/* rad/s angular velocity wrt inertial frame */

	double Ja;					/* -- constant in kinetic energy equation*/
	double Jb;					/* -- constant in kinetic energy equation*/
	double Jx;					/* -- constant in kinetic energy equation*/

	double mu1;					/* -- constant in potential energy equation*/
	double mu2;					/* -- constant in potential energy equation*/
	
	double angular_momentum1;			/* kg*m^2/s angular momentum of pendulum 1*/
	double angular_momentum2;			/* kg*m^2/s angular momentum of pendulum 2*/
	double total_angular_momentum;			/* kg*m^2/s system's total angular momentum*/
	double kinetic_energy;				/* N*m  kinetic energy */	
	double potential_energy;			/* N*m  potential energy */
	double total_energy;				/* N*m  total energy */
	double omegaDot1;				/* rad/s2 angular acceleration wrt inertial frame */
	double omegaDot2;				/* rad/s2 angular acceleration wrt inertial frame */

	double inertia_tensor1[3][3];			/* kg*m^2 inertia tensor */
	double inertia_tensor2[3][3];			/* kg*m^2 inertia tensor */
			
	double radius1;					/* m radius length */
	double length1;					/* m pendulum 1 length */
	double length2;					/* m pendulum 2 length */
	double radius2;					/* m radius length */
	double mass1;					/* kg mass of object */	
        double mass2;					/* kg mass of object */
	double inertia1;				/* kg*m^3 */
	double inertia2;				/* kg*m^3 */
	double mat_mass[6][6];				/* -- mass matrix */
	double quad1_mat[3][3];				/* -- mass matrix */
	double quad2_mat[3][3];				/* -- mass matrix */
	double quad3_mat[3][3];				/* -- mass matrix */
	double quad4_mat[3][3];				/* -- mass matrix */
	double matrix_a[2][2];				/* -- mass matrix */
	double I_matrix[2][2];				/* -- inertia matrix */

	double CM[3]; 
	double CM1[3]; 
	double CM2[3]; 

	double vec_a[6];
	double vec_b[6];

	double rotate1[3][3];
	double rotate2[3][3];
		
	double gravity;					/*m/s^2 acceleration due to gravity*/
	double force_remain;
	double torque_remain;

	// Methods
	void default_data();
	void init();
	void derivative();
	int integ();

	void mass_matrix();	
	void calcposition();
	void calcforce_remain();
	void calctotal_energy();
        void calctotal_angular_momentum();
	void calctorque_remain();
	void eq_setup();
	void eq_solver();
};

#endif
