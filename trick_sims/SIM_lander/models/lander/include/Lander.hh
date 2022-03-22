/************************************************************************
PURPOSE: (Simulate a lander.)
LIBRARY DEPENDENCIES:
    ((lander/src/Lander.o))
**************************************************************************/
#ifndef _lander_hh_
#define _lander_hh_

#include "PIDController/include/PIDController.hh"

class Lander {

    public:
        double pos[2] ;    /* (m)     xy-position */
        double vel[2] ;    /* (m/s)   xy-velocity */
        double acc[2] ;    /* (m/s2)  xy-acceleration  */

        double pitch;      /* (rad)    pitch angle */
        double pitchDot;   /* (rad/s)  pitch angular-rate */
        double pitchDDot;  /* (rad/s2) pitch angular-acceleration */

        double nozzle_angle;       /* (rad) */
        double main_engine_offset; /* (m) distance of main engine swivel from vehicle center of gravity. */
        double thrust_max;         /* (N) Maximum thrust that the main engine can produce. */
        double rcs_thrust;         /* (N) Thrust produced when RCS motor is "On". */
        double rcs_offset;         /* (m) distance of RCS motors from vehicle center of gravity. */
        double mass;               /* (kg) */
        double moment_of_inertia;  /* (kg/m2) */
        double g;                  /* (m/s2) - acceleration of gravity. */
        int throttle;              /* -- Percentage of thrust_max. Range: 0 .. 100. */
        int rcs_command;           /* -- -1 = counter-clockwise, 0 = no torque, 1 = clockwise */

        // Controls from the client
        int manual_throttle_change_command;     /* add to throttle */
        int manual_rcs_command;                 /* -1 = counter-clockwise, 0 = no torque, 1 = clockwise */

        int altitudeCtrlEnabled;                /* -- 0= disabled, 1=ensabled */
        int downRangeCtrlEnabled;

        PIDController* posxCntrl;
        PIDController* posyCntrl;
        PIDController* energy_controller;
        PIDController* velxCntrl;
        PIDController* velyCntrl;
        PIDController* nozzlePitchController;
        PIDController* nozzlePitchRateController;
        PIDController* RCSpitchController;
        PIDController* RCSpitchRateController;

        double RCS_pitch_dot_limit;
        double RCS_pitch_ddot_limit;

        double x_cmd;
        double y_cmd;
        double vy_cmd;
        double vx_cmd;
        double ay_cmd;
        double ax_cmd;
        double pitch_cmd;
        double pitch_dot_cmd;
        double pitch_ddot_cmd;

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
        int check_ground_contact();
        int control();
};
#endif
