#include "ElectricalCircuit.hh"
#include "../../Battery/include/DCBattery.hh"
#include "../../Motor/include/DCMotor.hh"
#include <iostream>

ElectricalCircuit::ElectricalCircuit()
    : motorsCurrent(0),
      motor1(0),
      motor2(0),
      battery(0)
{
}

void ElectricalCircuit ::init(DCMotor * motorOne, DCMotor * motorTwo, DCBattery * battery1)
{
    motor1 = motorOne;
    motor2 = motorTwo;
    battery = battery1;
}

void ElectricalCircuit ::update()
{
    motorsCurrent = motor1->getCurrentLoad() + motor2->getCurrentLoad();
    battery->setCurrent(motorsCurrent);
}
