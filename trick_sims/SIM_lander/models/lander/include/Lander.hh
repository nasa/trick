/************************************************************************
PURPOSE: (Simulate a lander.)
LIBRARY DEPENDENCIES:
    ((lander/src/Lander.o))
**************************************************************************/
#ifndef _lander_hh_
#define _lander_hh_

class PIDController {
    public:
    double Kp;
    double Kd;
    double Ki;
    double Dt;
    double error;
    double previous_error;
    double integral;
    double derivative;
    //int    first_time;
    double integral_max;
    double prev_set_point_value;

    PIDController( double kp, double ki, double kd, double imax, double dt );
    double getOutput(double commanded_value, double measured_value);
};

class Lander {

    public:
        double pos[2] ;    /* (m)     xy-position */
        double vel[2] ;    /* (m/s)   xy-velocity */
        double acc[2] ;    /* (m/s2)  xy-acceleration  */
        double angle;
        double angleDot;
        double angleDDot;
        double thrust_max;         /* (N) Maximum thrust that the main engine can produce. */
        double rcs_torque;         /* */
        double mass;               /* (kg) */
        double moment_of_inertia;  /* (kg/m2) */
        double g;                  /* (m/s2) - acceleration of gravity. */
        int throttle;
        int rcs_command;

        // Controls from the client
        int manual_throttle_change_command;       /* add to throttle */
        int manual_rcs_command;                 /* -1 = counter-clockwise, 0 = no torque, 1 = clockwise */
        int altitudeCtrlEnabled;
        int downRangeCtrlEnabled;

        PIDController* posxCntrl;
        PIDController* posyCntrl;

        PIDController* thrust_controller;

        PIDController* velxCntrl;
        PIDController* velyCntrl;
        PIDController* angleCntrl;

        double x_cmd;
        double y_cmd;
        double vy_cmd;
        double vx_cmd;
        double ay_cmd;
        double ax_cmd;
        double angle_cmd;
        double angle_dot_cmd;

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
        int check_ground_contact();
        int control();
};
#endif
