/**************************************************************
PURPOSE:              (Electrical Model)
***************************************************************/
#ifndef ELECTRICAL_CIRCUIT_H
#define ELECTRICAL_CIRCUIT_H

class DCBattery;
class DCMotor;

class ElectricalCircuit
{
public:
    DCMotor*        motor1;
    DCMotor*        motor2;
    DCBattery*     battery;
    ElectricalCircuit();
    void init (DCMotor* motor1, DCMotor* motor2, DCBattery* battery);
    void update();
private:
    double motorsCurrent;

};

#endif
