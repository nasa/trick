#ifndef TEST_VEHICLE_HH
#define TEST_VEHICLE_HH
/*************************************************************************
PURPOSE: ()
**************************************************************************/
#include "Guidance/include/point.hh"
#include "Control/include/vehicleController.hh"
#include "Control/include/differentialDriveController.hh"
#include "Motor/include/DCMotorSpeedController.hh"

class VehicleOne {
    public:
    std::vector<Point> waypointQueue;
    Navigator *navigator;
    MotorSpeedController* rightMotorController;
    MotorSpeedController* leftMotorController;
    DCMotor* rightDCMotor;
    DCMotor* leftDCMotor;

    DifferentialDriveController* driveController;
    VehicleController* vehicleController;

    double distanceBetweenWheels;     /* m */
    double wheelRadius;               /* m */
    double vehicleMass;               /* kg */
    double ZAxisMomentofInertia;

    // Vehicle Controller Parameters
    double slowDownDistance;          /* m */
    double arrivalDistance;           /* m */
    double wheelSpeedLimit;           /* rad/s */
    double headingRateLimit;          /* rad/s */
    double wheelDragConstant;         /* -- */
    double corningStiffness;          /* -- */

    // DCMotor Parameters
    double DCMotorInternalResistance; /* ohm */
    double DCMotorTorqueConstant;     /* N*m/amp */

    double position[2];              /* m */
    double velocity[2];              /* m/s */
    double acceleration[2];          /* m/s2 */

    double heading;                  /* rad */
    double headingRate;              /* rad/s */
    double headingAccel;             /* rad/s2 */

    double rightMotorSpeed;          /* rad/s */
    double leftMotorSpeed;           /* rad/s */

    // Forces
    double driveForce[2];            /* N */
    double lateralTireForce[2];      /* N */
    double rollingResistForce[2];    /* N */
    double forceTotal[2];            /* N */
    double vehicleZTorque;           /* N*m */

    double batteryVoltage;

    // Homing Button Variables
    // Get input from Trick server client for homing
    int homeCommanded;
    // If Wheelbot was homed and end of simulation
    bool endofHoming;

    void add_waypoint(double x, double y);

    int default_data();
    int state_init();
    void control();
    int state_deriv();
    int state_integ();
};

#endif
