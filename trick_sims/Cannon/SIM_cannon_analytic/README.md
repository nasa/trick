# SIM\_cannon\_analytic
---
This is first of eight Trick-based simulations that one builds in the Trick
Tutorial (Section 3). It's purpose is to introduce some of the fundamentals
of building a Trick simulation.

Here we simulate the flight of a cannon ball. We want to know the position and velocity of the cannon ball over time, given an initial position, and
velocity, and subject to the following assumptions and limitations:

* The **only** force acting on the cannon ball is gravity.
* The acceleration of gravity (g) is **constant** and equal to -9.81 meters per
  second squared.
* The surface of the ground is defined as where y=0.

![](images/CannonInit.png)

### Solution

This problem has a closed-form solution, so that's what is used.

<!--
Tex: v_{x0}=S\cos\theta
-->
![](images/init_v_x_0.png)

<!--
Tex: v_{y0}=S\sin\theta
-->
![](images/init_v_y_0.png)

![](images/init_a_x.png)

![](images/init_a_y.png)

![](images/solution_vx.png)

![v_{y}(t) = gt +v_{y0}](images/solution_vy.png)

![](images/solution_x.png)

![](images/solution_y.png)


The cannon ball will impact the ground, when y(t)=0 at:

![](images/time_of_impact.png)


### CANNON Object
Model Variable                              | Simulation Variable | Type    | Units
--------------------------------------------|---------------------|---------|-------
![](images/x_0.png), ![](images/y_0.png)    | CANNON.pos0[2]      |double[2]| m
![](images/v_x_0.png), ![](images/v_y_0.png)| CANNON.vel0[2]      |double[2]| m/s
![\theta](images/param_theta.png)           | CANNON.init\_angle  |double   | r
![speed](images/param_s.png)                | CANNON.init\_speed  |double   | m/s
![\vec{x}](images/vector_x.png)             | CANNON.pos[2]       |double[2]| m
![\vec{v}](images/vector_v.png)             | CANNON.vel[2]       |double[2]| m/s

