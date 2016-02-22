#ifndef TEST_MOTOR_CONTROLLER_HH
#define TEST_MOTOR_CONTROLLER_HH
#include "Motor/include/motorSpeedController.hh"

class TestMotorController : public MotorSpeedController {

    // This class is for testing.
    // It plays the roll of a Controller and a Motor.
    public:
        TestMotorController(): motorSpeed(0.0) {}

        void setCommandedSpeed( double speed_command) {
            commandedSpeed = speed_command;
        }

    private:
        double commandedSpeed; // rad/s
};
#endif
