/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate a satellite orbiting the Earth. )
LIBRARY DEPENDENCY:
    ((Lander.o))
*******************************************************************************/
#include "../include/Lander.hh"
#include "trick/integrator_c_intf.h"
#include <math.h>
#include <iostream>

#define RAD_PER_DEG (M_PI / 180.0)
#define DEG_PER_RAD (180.0/ M_PI )

PIDController::PIDController( double kp, double ki, double kd, double imax, double dt ) {
    Kp=kp; Ki=ki; Kd=kd; Dt=dt;
    previous_error = 0;
    integral = 0;
    integral_max = imax;
    prev_set_point_value = 0.0;
}

double PIDController::getOutput(double set_point_value, double measured_value) {

    error = set_point_value - measured_value;

    if (prev_set_point_value != set_point_value) {
        previous_error = error;
    }

    integral = integral + error * Dt;
    if (integral < -integral_max) integral = -integral_max;
    if (integral >  integral_max) integral =  integral_max;
    derivative = (error - previous_error) / Dt;
    previous_error = error;
    prev_set_point_value = set_point_value;

    double pterm = Kp * error;
    double iterm = Ki * integral;
    double dterm = Kd * derivative;
    double output = pterm + iterm + dterm;

    return ( output );
}

int Lander::default_data() {

    pos[0] = 0.0;
    pos[1] = 1.8;
    vel[0] = 0.0;
    vel[1] = 0.0;
    angle = 0.0;
    angleDot = 0.0;
    angleDDot = 0.0;

    thrust_max = 15000;
    rcs_torque = 50;
    mass = 2000;
    moment_of_inertia = 2000;
    g = 1.62;                  // Gravitional acceleration on the moon.

    throttle = 0;
    rcs_command = 0;

    // Controls
    manual_throttle_change_command = 0;
    manual_rcs_command = 0;
    altitudeCtrlEnabled = 0;
    downRangeCtrlEnabled = 0;

    // Auto Control Parameters
    x_cmd = 40.0;
    y_cmd = 0.0;

    return (0);
}

int Lander::state_init() {

    posxCntrl  = new PIDController( 0.10, 0.01, 0.60, 10.0, 0.10 );
    velxCntrl  = new PIDController( 0.10, 0.00, 0.60, 10.0, 0.10 );

    posyCntrl  = new PIDController( 0.10, 0.01, 0.60, 10.0, 0.10 );
    velyCntrl  = new PIDController( 0.50, 0.00, 1.40, 10.0, 0.10 );

    angleCntrl = new PIDController( 0.20, 0.00, 0.80, 10.0, 0.10 );

    return (0);

}

#define MAX_CTRL_ANGLE 30.0
int Lander::control() {

    if (altitudeCtrlEnabled == 1) {
         vy_cmd = posyCntrl->getOutput(y_cmd, pos[1]);
         ay_cmd = g + velyCntrl->getOutput(vy_cmd, vel[1]);
         throttle = (ay_cmd * mass * 100) / thrust_max;
    }

    if (downRangeCtrlEnabled == 1) {

        if (throttle >= 10 ) {
            vx_cmd = posxCntrl->getOutput(x_cmd, pos[0]);
            double thrust = (throttle / 100.0) * thrust_max;
            double a_max = thrust / mass;
            ax_cmd = velxCntrl->getOutput(vx_cmd, vel[0]);
            if (ax_cmd >  0.707 * a_max ) ax_cmd =  0.707 * a_max ;
            if (ax_cmd < -0.707 * a_max ) ax_cmd = -0.707 * a_max ;
            double acc_ratio = ax_cmd / a_max ; // must be between 0..1
            angle_cmd = -asin( acc_ratio );
        } else {
            angle_cmd = 0.0;
        }

        if (angle_cmd >  MAX_CTRL_ANGLE * RAD_PER_DEG) angle_cmd =  MAX_CTRL_ANGLE * RAD_PER_DEG;
        if (angle_cmd < -MAX_CTRL_ANGLE * RAD_PER_DEG) angle_cmd = -MAX_CTRL_ANGLE * RAD_PER_DEG;

        angle_dot_cmd = angleCntrl->getOutput(angle_cmd, angle);

        if ((angle_dot_cmd - angleDot) > 0.1 * RAD_PER_DEG) {
            rcs_command =  1;
        } else if ((angle_dot_cmd - angleDot) < -0.1 * RAD_PER_DEG) {
            rcs_command = -1;
        } else {
            rcs_command =  0;
        }

    } else {
        rcs_command = manual_rcs_command;
    }

    throttle += manual_throttle_change_command;
    if (throttle > 100) throttle = 100;
    if (throttle <   0) throttle = 0;

    return (0);
}

int Lander::state_deriv() {

   double thrust = (throttle / 100.0) * thrust_max;

   acc[0] = (thrust * -sin(angle)) / mass;
   acc[1] = (thrust *  cos(angle)) / mass - g;

   angleDDot = rcs_command * rcs_torque / moment_of_inertia;

   return(0);
}

int Lander::state_integ() {

   int integration_step;

   load_state ( &pos[0], &pos[1], &vel[0], &vel[1], &angle, &angleDot, (double*)0);
   load_deriv ( &vel[0], &vel[1], &acc[0], &acc[1], &angleDot, &angleDDot, (double*)0);

   integration_step = integrate();

   unload_state( &pos[0], &pos[1], &vel[0], &vel[1], &angle, &angleDot, (double*)0);

   return(integration_step);
}

int Lander::check_ground_contact() {
    if (pos[1] < 1.8) {
        pos[1] = 1.8;
        vel[0] = 0.0;
        vel[1] = 0.0;
        angle = 0.0;
        angleDot = 0.0;
    }
    return(0);
}
