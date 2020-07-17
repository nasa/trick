# SIM\_wheelbot

---

SIM\_wheelbot is a simulation of a two-wheeled electric vehicle that follows a set of waypoints.

![Picture of Vehicle](images/Figure1.png)
---
### Building the Simulation
In the SIM\_wheelbot directory, type **trick-CP** to build the simulation executable. When it's complete, you should see:

```
=== Simulation make complete ===
```

Now **cd** into models/Graphics/ and type **make**. This builds the display client for the simulation.

### Running the Simulation
In the SIM_wheelbot directory:

```
% S_main_*.exe RUN_test/input.py
```

---

### Inputs
Variable                                       | Type           | Units | Default Value
-----------------------------------------------|----------------|-------|--------------
veh.vehicle.distanceBetweenWheels              | double         | m     | 0.183
veh.vehicle.wheelRadius                        | double         | m     | 0.045
veh.vehicle.wheelSpeedLimit                    | double         | rad/s | 8.880
veh.vehicle.headingRateLimit                   | double         | rad/s | 𝛑/4
veh.vehicle.wheelDragConstant                  | double         | --    | 1.875
veh.vehicle.corningStiffness                   | double         | --    | 10.0
veh.vehicle.slowDownDistance                   | double         | --    | 0.5
veh.vehicle.arrivalDistance                    | double         | --    | 0.1

![Picture of Vehicle](images/Figure2.png)

#### Adding Waypoints
Waypoints, for the vehicle to follow, are added with a call to

veh.vehicle.add_waypoint( double N, double W )

and HomePoint for the vehicle to home to is called to

veh.Vehicle.add_homepoint( double N, double W)


#### Enabling or Disabling Homing
To set a HomePoint:

Edit the fuction veh.vehicle.add_homepoint( float(1.5), float(-1.5)) in the input.py file under RUN_test.

Replace the float values with desired home location.

Under models/Control/src/vehicleController.cpp, locate the declaration:

homing = true;

To disable homing, comment out the line of code.


### Input/Output
Variable                                       | Type           | Units
-----------------------------------------------|----------------|-------
veh.vehicle.position                           | double[2]      | m
veh.vehicle.velocity                           | double[2]      | m
veh.vehicle.heading                            | double         | rad
veh.vehicle.headingRate                        | double         | rad


### Outputs
Variable                                      | Type           | Units
----------------------------------------------|----------------|--------
veh.vehicle.acceleration                      | double[2]      | m
veh.vehicle.headingAccel                      | double         | rad
veh.vehicle.driveForce                        | double[2]      | N
veh.vehicle.lateralTireForce                  | double[2]      | N
veh.vehicle.rollingResistForce                | double[2]      | N
veh.vehicle.forceTotal                        | double[2]      | N
veh.vehicle.vehicleZTorque                    | double         | N*m
