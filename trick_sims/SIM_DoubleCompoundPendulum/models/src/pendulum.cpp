/*************************************************************************
PURPOSE: (Double Compound Pendulum Simulation)
LIBRARY DEPENDENCIES: 
	((models/src/pendulum.o))
**************************************************************************/

#include <math.h>
#include <cmath>
#include <stdexcept>

#include "trick/integrator_c_intf.h"
#include "trick/trick_math_proto.h"
#include "trick/trick_math_error.h"
#include "trick/reference_frame.h"
#include "trick/vector_macros.h"
#include "trick/matrix_macros.h"

#include "../include/pendulum.hh"

PENDULUM::PENDULUM(){};
PENDULUM::~PENDULUM(){};

/*************************************************************************
FUNCTION: PENDULUM::default_data()
PURPOSE:` (Creates an object)
*************************************************************************/

void PENDULUM::default_data() {

	//Initializing vectors and matricies to zero. 
	for(int i = 0; i<6; i++)
		vec_b[i] = 0.0;

	pos1[0] = 0.0;
	pos1[1] = 0.0;
	pos2[0] = 0.0;
	pos2[1] = 0.0;
	vel1[0] = 0.0;
	vel1[1] = 0.0;
	vel2[0] = 0.0;
	vel2[1] = 0.0;
	angle1 = 0.0;
	length1 = 0.0;
	length2 = 0.0;
	angle2 = 0.0;
	omega1 = 0.0;
	omega2 = 0.0;
	omegaDot1 = 0.0;
	omegaDot2 = 0.0;
	inertia1 = 0.0;
	inertia2 = 0.0;
	Ja = 0.0;
	Jb = 0.0;
	Jx = 0.0;
	mu1 = 0.0;
	mu2 = 0.0;

};

/************************************************************************
FUNCTION: PENDULUM::init()
PURPOSE: (Initializing state variables, creating inertia and CoM skew 
	  matrices) 
************************************************************************/
void PENDULUM::init() {

	pos1[0] = POS1_INIT[0];
	pos1[1] = POS1_INIT[1];
	pos2[0] = POS2_INIT[0];
	pos2[1] = POS2_INIT[1];
	vel1[0] = VEL1_INIT[0];
	vel1[1] = VEL1_INIT[1];
	vel2[0] = VEL2_INIT[0];
	vel2[1] = VEL2_INIT[1];

        angle1 = ANGLE1_INIT;
	angle2 = ANGLE2_INIT;
	omega1 = OMEGA1_INIT;
	omega2 = OMEGA2_INIT;
	omegaDot1 = OMEGADOT1_INIT;
	omegaDot2 = OMEGADOT2_INIT;

};

/******************************************************************************
FUNCTION: PENDULUM::mass_matrix()
PURPOSE: (Calculates 6x6 mass matrix M for eq M*a + r = b)
******************************************************************************/
void PENDULUM::mass_matrix(){
	matrix_a[0][0] = Ja;
	matrix_a[0][1] = Jx * cos(angle1 - angle2);
	matrix_a[1][0] = Jx * cos(angle1 - angle2);
	matrix_a[1][1] = Jb;
};

void PENDULUM::calcposition() {
	pos1[0] = length1 * sin(angle1);
        pos1[1] = -length1 * cos(angle1);

	pos2[0] = (length1 * sin(angle1)) + (length2 * sin(angle2));
	pos2[1] = (-length1 * cos(angle1)) + (-length2 * cos(angle2));
};

/****************************************************************************
FUNCTION: PENDULUM::calcforce_remain()
PURPOSE: (Calculate remaining force wrt inertial frame)
****************************************************************************/
void PENDULUM::calcforce_remain() {
	force_remain = Jx * sin(angle1 - angle2) * omega2 * omega2 + (mu1 * sin(angle1));
};

/***************************************************************************
FUNCTION: PENDULUM::calctorque_remain()
PURPOSE: (Calculate remaining torque wrt inertial frame)
***************************************************************************/
void PENDULUM::calctorque_remain() {
	torque_remain = mu2 * sin(angle2) - (Jx * sin(angle1 - angle2) * omega1 * omega1);
};

/***************************************************************************
FUNCTION: PENDULUM::eq_setup()
PURPOSE: (Establishes b column vector for final eq M*a=b)
***************************************************************************/
void PENDULUM::eq_setup() {
	vec_b[0] = -force_remain;
	vec_b[1] = -torque_remain;
};

/**************************************************************************
FUNCTION: PENDULUM::eq_solver()
PURPOSE: (Performs choleski decomposition and computes linear and angular
          accelerations)
**************************************************************************/
void PENDULUM::eq_solver() {
	// Extract elements
	double a = matrix_a[0][0];
	double b = matrix_a[0][1];
	double c = matrix_a[1][0];
	double d = matrix_a[1][1];
	
	// Calculate determinant
	double det = (a * d) - (b * c);
	
	// Check if determinant is zero
	if (det == 0) {
	    throw std::invalid_argument("Matrix is singular and cannot be inverted.");
	}

	// Calculate the inverse
	double I_matrix[2][2];
	I_matrix[0][0] = d / det; 
	I_matrix[0][1] = -1.0 * b / det;
        I_matrix[1][0] = -1.0 * c / det;
	I_matrix[1][1] = a / det;

	// Calculate angular acceleration
	omegaDot1 = (I_matrix[0][0] * vec_b[0]) + (I_matrix[0][1] * vec_b[1]);
	omegaDot2 = (I_matrix[1][0] * vec_b[0]) + (I_matrix[1][1] * vec_b[1]);
};



/***************************************************************************
FUNCTION: PENDULUM::calctotalenergy()
PURPOSE: (Calculates kinetic, potential, and total energy in the system)
***************************************************************************/
void PENDULUM::calctotal_energy(){
	kinetic_energy = (0.5*omega1*omega1*(Ja))+(0.5*(Jb)*omega2*omega2)+(Jx*omega1*omega2*cos(angle1 - angle2));
	potential_energy = (-mu1*cos(angle1))-(mu2*cos(angle2));
	total_energy =  kinetic_energy + potential_energy;
};
			
/***************************************************************************
FUNCTION: PENDULUM::calctotalangularmomentum()
PURPOSE: (Calculates individual and total angular momentum in the system)
***************************************************************************/
void PENDULUM::calctotal_angular_momentum(){
	angular_momentum1 = inertia1*omega1;
        angular_momentum2 = inertia2*omega2;
        total_angular_momentum = angular_momentum1+angular_momentum2;

};

/***************************************************************************
FUNCTION: PENDULUM::derivative()
PURPOSE: (Calls all functions in desired order for calculations)
***************************************************************************/
void PENDULUM::derivative() {
	mass_matrix();
	calcforce_remain();
	calctorque_remain();
	eq_setup();
	eq_solver();
	calcposition();
	calctotal_angular_momentum();
	calctotal_energy();
};

/***************************************************************************
FUNCTION: PENDULUM::integrate()
PURPOSE: (Sets up trick integration)
***************************************************************************/

#ifndef IN_MAKE
int PENDULUM::integ() {

	int integration_step;

        load_state(
        &angle1,
	&omega1,
	&angle2,
	&omega2,
        NULL);

        load_deriv(
        &omega1,
	&omegaDot1,
	&omega2,
	&omegaDot2,
        NULL);

        integration_step = integrate();

        unload_state(
        &angle1,
	&omega1,
	&angle2,
	&omega2,
        NULL);


        return(integration_step);
};

#endif
