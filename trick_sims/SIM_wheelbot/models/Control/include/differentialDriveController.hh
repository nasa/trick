/*
 * PURPOSE: (DifferentialDriveController class)
 */
#ifndef DIFFERENTIAL_DRIVE_CONTROLER_HH
#define DIFFERENTIAL_DRIVE_CONTROLER_HH

#include "Motor/include/motorSpeedController.hh"

class DifferentialDriveController {

    public:
        DifferentialDriveController( double distanceBetweenWheels,
                                     double wheelRadius,
                                     double wheelSpeedLimit,
                                     double headingRateLimit,
                                     double slowDownDistance,
                                     MotorSpeedController& rightMotorController,
                                     MotorSpeedController& leftMotorController
                                   );

        int update( double distance_err,
                    double heading_err);
        void stop();

    private:

        // Do not allow the default constructor to be used.
        DifferentialDriveController();

        double distanceBetweenWheels;
        double wheelRadius;
        double wheelSpeedLimit;
        double headingRateLimit;
        double slowDownDistance;

        MotorSpeedController& rightMotorController;
        MotorSpeedController& leftMotorController;

        double rightMotorSpeedCommand;
        double leftMotorSpeedCommand;
        double desiredHeadingRate;
        double desiredRangeRate;
};
#endif
