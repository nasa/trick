# class PIDController

![PID Controller Diagram](images/PIDController.png)

### Constructor

```
PIDController::PIDController(
    double kp,   // proportional gain
    double ki,   // integral gain
    double kd,   // derivative gain
    double omax, // limiter maximum
    double omin, // limiter minimum
    double dt,   // sample period
    double tc    // filter time constant
    );
```


For no filtering, set tc to the value of dt. To filter, set tc a value higher than dt.

               
```           
double PIDController::getOutput( double setpoint_value,
                                 double measured_value);
```
This function generates the control command (corresponding to **cmd** in the above diagram.) for the plant. The "plant" is the thing that you are controlling.

For example, the plant could be a lunar lander. The measured value could be altitude, and the command could be motor thrust. The setpoint is the what you want the output of the plant to be. The setpoint could be an altitude of 100 feet. The measured_value is the actual output (the actual altitude) of the lander plus perhaps some noise (because sensors aren't perfect).

### References

The following videos provide some excellent instruction about PID control:

[Understanding PID Control, Part 1: What is PID Control?](https://www.youtube.com/watch?v=wkfEZmsQqiA)

[Understanding PID Control, Part 2: Expanding Beyond a Simple Integral](https://www.youtube.com/watch?v=NVLXCwc8HzM)

[Understanding PID Control, Part 3: Part 3: Expanding Beyond a Simple Derivative](https://www.youtube.com/watch?v=7dUVdrs1e18)

[Understanding PID Control, Part 4: A PID Tuning Guide](https://www.youtube.com/watch?v=sFOEsA0Irjs)
