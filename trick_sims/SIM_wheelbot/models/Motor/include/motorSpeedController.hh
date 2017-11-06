/*
 * PURPOSE: (MotorSpeedController class)
 */
#ifndef MOTOR_SPEED_CONTROLLER_HH
#define MOTOR_SPEED_CONTROLLER_HH

class MotorSpeedController {

    public:
    virtual ~MotorSpeedController() {}

    virtual void setCommandedSpeed( double commanded_speed ) {
        commandedSpeed = commanded_speed;
    }
    double getCommandedSpeed() { return commandedSpeed; }

    protected:
    double commandedSpeed;
};

#endif
