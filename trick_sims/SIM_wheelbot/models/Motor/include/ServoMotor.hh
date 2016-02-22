#ifndef SERVO_MOTOR_HH
#define SERVO_MOTOR_HH

class ServoMotor
{
  public:
    ServoMotor (char side);
    double getActualSpeed( int PulseWidth );
  private:
    ServoMotor();
    char _side;
    int _PulseWidth;
    double actualspeed;
}

#endif