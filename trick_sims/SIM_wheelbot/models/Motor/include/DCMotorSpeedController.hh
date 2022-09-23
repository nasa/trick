/*
 * PURPOSE: (DCMotorSpeedController class)
 */
#ifndef DC_MOTOR_SPEED_CONTROLLER_HH
#define DC_MOTOR_SPEED_CONTROLLER_HH

#include "motorSpeedController.hh"
#include "DCMotor.hh"

class DCMotorSpeedController : public MotorSpeedController {

    public:
    DCMotorSpeedController( DCMotor& dc_motor,
                            double gain,
                            const double& actual_speed,
                            const double& supply_voltage);

    ~DCMotorSpeedController() {}

    void setCommandedSpeed( double commandedSpeed );
    double getMotorVoltage();

    private:

    double motorVoltage;
    DCMotor& motor;
    double gain;
    const double& actualSpeed;
    const double& supplyVoltage;

    // Don't Allow the default constructor to be used.
    DCMotorSpeedController();
};

#endif
