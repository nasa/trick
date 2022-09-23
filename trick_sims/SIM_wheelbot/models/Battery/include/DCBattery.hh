/******************************************
 PURPOSE:	(DCBattery)
 *******************************************/
#ifndef DCBattery_H
#define DCBattery_H

class DCBattery
{
public:
    DCBattery(double initialIdealVoltage, double initialInternalResistance);
    void update ();
    double getActualVoltage();
    void setCurrent(double current);
private:
    double idealVoltage;
    double internalResistance;
    double actualVoltage;
    double current;
};
#endif
