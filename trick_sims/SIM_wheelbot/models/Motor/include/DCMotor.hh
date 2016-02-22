#ifndef DCMotor_H
#define DCMotor_H
/********************************* TRICK HEADER *******************************
PURPOSE: ()
LIBRARY DEPENDENCY:
    ((DCMotor.o))
*******************************************************************************/
#include "PWM.hh"

class DCMotor {
public:
    DCMotor(const double initialInternalResistance, const double initialMotorTorqueConstant);
    void update (const double motorVoltage);
    void update(const PWM& PulseWidth);
    double getTorque ();
    double getCurrentLoad ();

private:
    double motorTorque;
    double motorCurrent;
    double currentLoad;
    double internalResistance;
    double motorTorqueConstant;
};

#endif

