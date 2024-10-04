# Single Rigid Body
``SIM_singlerigidbody`` is a simulation of the motion of a sphere that is a single rigid body with six degrees of freedom. 

The motion of the sphere is found by using Euler's equations of motion, and can be controlled by changing several initial input values such as force, position, angle, angular velocity, velocity, and position of the force. The recorded data of the motion can then be viewed and analyzed. 

## Building the simulation
In the ```SIM_singlerigidbody```  directory, type **trick-CP** to build the simulation executable. When it is complete, you should see: 

```
=== Simulation make complete ===
```

## Running the simulation
In the ```SIM_singlerigidbody``` directory, type: 

```
% S_main_*.exe RUN_test/input.py
```

## Recorded data
To review the recorded data using trick quick plot, type:

```
trick-dp &
```
The quick plot should appear. 

To view the data: 
* Double click the path name containing the sim directory in the **Sims/Runs Tree** pane. 
* Double click **RUN_test** and click the blue lightning button in the tool bar to launch quick plot. 
* In quick plot, double click any variable in the **vars** pane and then click on the single white sheet in the tools bar to view the data. 

![dp_example](images\trick-dp.png)

### Alternative method
An alternative method to view the data is to use koviz. 

If not already installed, download from Github and follow the instructions to install: https://github.com/nasa/koviz

To run koviz, type: 
```
koviz RUN_test
```

Koviz should appear. 

To view the data, double click on any variable under the **Vars** pane. 

![Koviz_Example](images\koviz.png)

## Dynamics model

### Equations of Motion
To determine the sphere's motion, we will use Euler's equations of motion for a single rigid body. Euler's equations describe the combined translational and rotational dynamics of a rigid body in three-dimensional space. 

![Equations_of_motion](images\EquationsofMotion.png)

Where, 

![Force_and_Torque_eq](images\ForceTorque.png)
* is a 6x1 column vector. 
* F is the applied force.
* &#0964; is the torque.

![Mass_Matrix_eq](images\MassMatrix.png)
* is a 6x6 matrix. 
* m is the mass.
* I <sub> 3x3 </sub> is the identity matrix. 
* I is the inertia. 
* C <sub> m </sub> is the center of mass. 

![Acceleration_Omega](images\AccOmega.png)
* is a 6x1 column vector. 
* a<sub> o </sub> is the acceleration.

![Force_Torque_Remain](images\ForceTorqueRemain.png)
* is a 6x1 column vector. 
* &#0969; <sup> A </sup> is the angular velocity.


#### Force
The force acting on the sphere is calculated by the cross product of the rotation matrix and initial force. 

![Force](images\Force.png)

#### Torque
Torque is found by the cross product of the rotation matrix and the cross product of the position of force and initial force. 

![Torque](images\Torque.png)

#### Mass
The default value of the mass of the sphere is: 

![Mass](images\Mass.png)

#### Radius
The default value of the radius is: 

![Radius](images\Radius.png)

#### Center of mass
The center of mass of a sphere is equal to its radius. 

![Center_of_mass](images\Centerofmass.png)

#### Inertia
Inertia of a sphere is calculated by: 

![Inertia](images\Inertia.png)

#### Acceleration and angular acceleration
To determine the acceleration and angular acceleration, ```dLU_Choleski``` must be used. ```dLU_Choleski``` solves linear sets of equations in the form of [A]x = b and returns the vector x, which is the acceleration. It decomposes the symmetric, positive definite [A] matrix into a lower triangular form, where we can then substitute and our equation becomes: 

![Vector_a](images\Vectora.png)

The term 'a' is a 6x1 vector. Acceleration is equal to the first three values of vector a  and angular acceleration is equal to the last three values of vector a.

### State Integration
The state is defined by position, velocity, angle, and angular velocity. They are calculated by numerically integrating velocity, acceleration, angular velocity, and angular acceleration, respectively. 

#### Position
The position of the sphere is determined by integrating the velocity over time. 

![Position](images\Position.png)

#### Velocity
The velocity is determined by integrating the acceleration of the sphere over time. 

![Velocity](images\Velocity.png)

#### Angle
The angle is determined by integrating the angular velocity over time. 

![Angle](images\Angle.png)

#### Angular velocity
The angular velocity is determined by integrating the angular acceleration over time. 

![Angular_Velocity](images\Angularvelocity.png)



## Inputs

Variable                    |Type         |Units
----------------------------|-------------|---------
dyn.body.POS_INIT           |double[3]    |m
dyn.body.ANGLE_INIT         |double[3]    |rad
dyn.body.OMEGA_INIT         |double[3]    |rad*s^-1 
dyn.body.FORCE_INIT         |double[3]    |N
dyn.body.TORQUE_INIT        |double[3]    |N*m
dyn.body.VEL_INIT           |double[3]    |m/s
dyn.body.POS_FORCE          |double[3]    |m

## Outputs
                            
Variable                    |Type         |Units
----------------------------|-------------|--------
dyn.body.pos                |double[3]    |m
dyn.body.vel                |double[3]    |m/s
dyn.body.acc                |double[3]    |m/s^2
dyn.FORCE_INIT              |double[3]    |N
dyn.force                   |double[3]    |N
dyn.body.angle_force        |double[3]    |rad
dyn.body.omegaDot           |double[3]    |rad/^s2
dyn.body.omega              |double[3]    |rad*s^-1
dyn.body.rotate             |double[3]    |
