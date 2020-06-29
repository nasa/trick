# Motor

**Contents**

* [class MotorSpeedController](#class-MotorSpeedController)<br>
 - [class DCMotorSpeedController](#class-DCMotorSpeedController)<br>
* [class Motor](#class-Motor)<br>
 - [class DCMotor](#class-DCMotor)<br>
* [class PWM](#class-PWM)<br>

---

<a id=class-MotorSpeedController></a>
## class MotorSpeedController
*base class*

---

<a id=class-DCMotorSpeedController></a>
## class DCMotorSpeedController 
*derived from class MotorSpeedController*

### Description
A DCMotorSpeedController is a simple proportional controller for an
instance of class DCMotor. Given a commanded-speed it produces an
input voltage for the associated DCMotor instance.

### Constructor
```
    DCMotorSpeedController( DCMotor& dc_motor,
                            double gain,
                            const double& actual_speed,
                            const double& supply_voltage);
```
| Access | Member Name   | Type          | Units | Value  |
|--------|---------------|---------------|-------|--------|
| private| gain          | double        | --    | Parameter - Feedback gain |
| private| actualSpeed   | const double& | m/s   | Parameter - Reference to the actual motor speed. |
| private| supplyVoltage | const double& | volts | Reference to the power supply voltage. |
| private| motor         |               | --    | Parameter - Reference to the [DCMotor](#class-DCMotor) instance to be controlled. |

### Member Functions

```
    void setCommandedSpeed( double commandedSpeed );
```
| Access | Member Name   | Type  | Units  | Value  |
|--------|---------------|-------|--------|--------|
| private| motorVoltage  |double | volts  | [Eq#1] |

* [Eq#1] **motorVoltage** = supplyVoltage * gain * ( commandedSpeed - actualSpeed ) [ limited to +/- supplyVoltage ]

```
    double getMotorVoltage();
```
Return the motorVoltage that was calculated by the last call to setCommandedSpeed().

---

<a id=class-Motor></a>
## class Motor
*base class*

---

<a id=class-DCMotor></a>
## class DCMotor
*derived from class Motor*

Class DCMotor represents a simple model of a DC motor. Given an input voltage it calculates a 

1. current-load, and 
2. a motor-torque.

The model consists of a constant internal resistance that converts an input voltage to current, and a motor torque constant that converts the current to output torque. The output current load is the absolute value of the current.

### Constructor

```
DCMotor (const double initialInternalResistance,
         const double initialMotorTorqueConstant);
```

| Access | Member Name          | Type   | Units  | Value  |
|--------|----------------------|--------|--------|--------|
| private| internalResistance   | double | ohms   | Input  |
| private| motorTorqueConstant  | double | Nm/amp | Input  |


### Member Functions

```
void update (const double motorVoltage);
```

This method is to be called periodically to update the motor state.

| Access | Member Name   | Type   | Units  | Value  |
|--------|---------------|--------|--------|--------|
| private| motorCurrent  | double | amp    | [Eq#1] |
| private| motorTorque   | double | Nm     | [Eq#2] |
| private| currentLoad   | double | amp    | [Eq#3] |

* [Eq#1] **motorCurrent** = motorVoltage / internalResistance

* [Eq#2] **motorTorque** = motorCurrent / motorTorqueConstant

* [Eq#3] **currentLoad** = || motorCurrent ||


```
void update (const PWM& PulseWidth);
```

PulseWidth is converted to voltage [ PulseWidth.getAverageVoltage() ], and then the above method is called.

```
double getTorque();
```

Return the torque that was calculated by the last call to ```update()```.

```
double getCurrentLoad()
```

Return the current-load that was calculated by the last call to update(). An example use of this method might be to update a battery model.

---

<a id=class-PWM></a>
## class PWM

### Description
This class represents a PWM signal. Pulse Width Modulation (PWM) is a method of controlling electrical power (current x voltage) by repeatedly switching the supply voltage on and off, or between a high and low (typically 0) voltage. The proportion of time that the voltage is "high" is called the duty-cycle. The average output voltage is:

<a id=EQ1-AverageVoltage></a>
[Eq#1] **average_voltage** = ((highVoltage * dutyCycle + lowVoltage * (1 - dutyCycle)) / 2)


### Constructor

```
    PWM( double HighVoltage,
         double LowVoltage,
         double DutyCycle);
```


| Access  | Member Name      | Type   | Units  | Value  |
|---------|------------------|--------|--------|--------|
| public  | HighVoltage      | double | volts  |        |
| public  | LowVoltage       | double | volts  |        |
| private | dutyCycle        | double | volts  |        |

### Member Functions

```
void setDutyCycle( double DutyCycle);
```
Set the duty cycle to be used by getAverageVoltage().

```
double getDutyCycle() const;
```
Get current the duty cycle.

```
double getAverageVoltage() const;
```
Calculate and return the average_voltage by [Eq#1](#EQ1-AverageVoltage).
