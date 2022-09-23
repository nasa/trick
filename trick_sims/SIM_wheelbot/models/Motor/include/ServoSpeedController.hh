#ifndef SERVO_SPEED_CONTROLLER_HH
#define SERVO_SPEED_CONTROLLER_HH

#include "motorSpeedController.hh"
#include "ServoMotor.hh"

class ServoSpeedController : public MotorSpeedController
{
  public:
    ServoSpeedController (ServoMotor& Servo_Motor, char side);
    void setCommandedSpeed (double commandedSpeed);
  private:
    ServoSpeedController();
    ServoMotor& servo;
    char _side;
    double commanded_Speed;
    double _PulseWidth;
}

#endif
