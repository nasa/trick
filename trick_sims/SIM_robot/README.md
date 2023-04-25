# Planar Robotic Manipulator Simulation

SIM_robot is a 2-dimensional planar robotic simulation of a 2 degree of freedom manipulator.  It is a kinematic simulation, which means that it concerns itself solely with the position and velocity of the links but does not model or account for forces and torques.  This helps keep the simulation more simple and easier to comprehend, while also providing the necessary framework to serve as an adequate example of the general layout of a robotics simulation.  The controller described below and implemented in the simulation is both simple and extremely common.  It is used extensively in both kinematic and dynamic robotic modeling and control.

The simulation calculates both forward and inverse kinematics and implements some common control schemes to allow the user to control the manipulator in a variety of ways.  Only the tip (or end-effector) position of the manipulator is controlled, not the orientation of the last link.

Included is a graphics and user interface client which allows the user to view the motion of the robotic arm in the sim and to send moding and control inputs to the manipulator in real time.  A circle indicates the maximum extent of the workspace of the manipulator.


## Building the Simulation
In order to build a Trick simulation, the Trick-specific command trick-CP must be run to begin compilation process.  trick-CP is located in the Trick **```bin```** directory.  Simply run the trick-CP command in the SIM_robot directory.  Upon successful compilation, the following message will be displayed in the terminal:

```
=== Simulation make complete ===
```

The assocated graphics/user interface client should also be built automatically.

## Running the Simulation
In the SIM_robot directory:

```
% S_main_*.exe RUN_test/input.py
```
The Sim Control Panel, and a graphics client called "Robot Range" should appear.

The sim will come up in Freeze mode.  Click Start on the Trick Sim Control Panel and the simulation time should start advancing.

![](images/GraphicsClient.png)


## Control Modes

There are three control modes available for this manipulator:  Single, Manual, and EEPos.  Each will be described below.  Furthermore, the gui provides a toggle to enable tracing the end-effector position.

### Single Joint Mode
Single Joint Mode is the most straightfoward control mode in the simulation.  It simply commands a selected joint to move with the angular velocity commanded.  The user interface provides a button to select Single as your mode, a Joint select button to choose the desired joint, and a slider to command a desired joint velocity.

### Manual Mode
Manual Mode enables control of the motion of the end-effector with a desired velocity.  This mode commands the end-effector to move in the desired direction at the desired rate until either the velocity command is changed or the arm reaches a singularity (described below).  The gui has an "EE Velocity" interface for interacting in this mode.  The gray circle is like a dial, where the angle and location of the selected point in the circle determine the commanded direction and rate of the end-effector.  For example, clicking directly underneath the center of the circle on the edge will command the end-effector to move straight down as fast as the mode allows.  This input can be changed while moving, allowing the user to maneuver the end-effector however they see fit in real time.

### End-Effector Position Mode
This mode commands the end-effector to autonomously move to the selected point in the workspace.  The user clicks anywhere on the gui display and the end-effector will attempt to move to that location.  Singularities and reach limits may prevent it from reaching the point, however.  The manipulator makes no effort to avoid these.

## Kinematics of the System
The kinematics of a robotic manipulator describe both the position and velocity of the manipulator at any point on the robot.  Kinematics do not include accelerations, forces, or moments in their description.

In this sim, we will discuss both forward and inverse kinematics.  Forward kinematics give the position/velocity of any location of interest on the manipulator given a set of joint angles/velocities.  Inverse kinematics go in the reverse:  Given a desired location/velocity for some point of interest on the manipulator, the equations supply the necessary joint angles/velocities.

The position of the end-effector is highly non-linear and heavily coupled with respect to the joint angles required to produce said position.  However, the velocity of the end-effector is linearly related to the joint velocities required to produce it, which forms the basis of the controller described below.  The non-linearity of the position equations, and the linearity of the velocity equations, will be shown in the next sections.

### How to Layout Points, Frames, and Joint Angles
The position of the end-effector relative to some fixed Base frame location can be calculated by knowing the joint angles of the manipulator and the lengths of the manipulator links.  First we will assign some points of interest along the manipulator that need to be kept track of during the forward kinematics calcualtions.  We will define the point around which the first link's rotation is centered as point A, the point of the second link's rotation B, and the tip of the manipulator (our end-effector) as E.  The distance between A and B will be defined $L1$ and between B and E $L2$.

![](images/POIs.png)

But points of interest are of no use without frames of reference, so we need to define some frames in which we can define the relative locations of these points.  Let's define a fixed, static Base (or Origin) Frame Fa centered on point A.  This frame will give us a reference frame to keep track of the location of points A and B (point A won't move, and point B moves by changing the first joint angle).  However, point E moves if either joint moves.  We need to keep track of how the links change their orientation with respect to both the base frame and to each other.  To do this we assign frames attached to each link, that move along with it, called Fb.  The convention for this sim is to point the X-axis of each frame along the length of these straight links, so that the X-axis points from each point of interest to the next (A to B, B to E).  Finally we will attache a frame to the end-effector, Fe.`

![](images/Frames.png)


We also need to keep track of the joint angles, since they can and will change during the run (otherwise it's less a manipulator and more a sculpture).  Let's define the joint angles as $q1$ and $q2$, making 0 rotation align all three frames so that they are all oriented the same way.  This way any rotation of a link can be easily described with joint angles q1 and q2.

<figure of joint angles with frames with points>


Now everything of importance to the kinematics of the system has been identified.  The base and joint frames allow us to keep track of the relative location of the points on the manipulator with respect to each other and the fixed inertial reference frame.  The joint angles allow us to describe every possible location of each of the points in any frame.


### Position of the End-Effector
The position of the tip of manipulator can be described by calculating the vectors from points A to B and from B to E and adding them together.  Vectors can be added together if they are described in the same frame.  

The vector from points A to B is a function of the joint angle $q1$ and the length of the link $L1$ (described in the base frame O)

<figure of joint angle q1 and length L1 with cos and sin layed out>

The vector from points B to E is a function of the joint angle $q2$ and the length of the link $L2$ (described in the first link's frame D).

<figure of joint angle q2 and length L2 with cos and sin layed out>


### Velocity of the End-Effector

### The Jacobian

### Singularities


## Control Schemes

### Single Joint Mode

### Manual Mode

### End-Effector Position Mode

### Singularity Management












### Manual Control of the Aircraft11
The two sliders on the bottom left and right are for setting the aircraft's desired speed and heading, respectively. The desired speed can range from 0 to 250 meters per second. The desired heading is measured in radians, from - $\pi$ to $\pi$.

### Aircraft Autopilot
The autopilot feature is toggled on and off by the ```Autopilot OFF/ON``` button at the bottom of the graphics client. 

![](images/GraphicsClientAutopilot.png)

When active, the aircraft's heading is automatically calculated to approach a series of waypoints. These waypoints are marked on the map by the image provided in the input file.

#### Adding Waypoints with an Input File
Waypoints can be added to the simulation using an input file specified in `input.py`. 

![](images/WaypointInputFile.png)

When the simulation starts they are read into the simulation from the specified file. Currently that file is `default.waypoints`  within the `Modified_data` folder. 

#### Sample Input in [`default.waypoints`](Modified\_data/default.wps):

         0.0,  25000.0,images/wp0.png
     21650.0,  12500.0,images/wp1.png
     21650.0, -12500.0,images/wp2.png
         0.0, -25000.0,images/wp3.png
    -21650.0, -12500.0,images/wp4.png
    -21650.0,  12500.0,images/wp5.png
Each line should have three pieces of data, separated by commas. The first two are the coordinates. They are formatted like GPS coordinates, with the first number being the vertical distance, or 'North', and the second being the horizontal, or 'West', distance. The third part of the line is the path to the image that will be used as the waypoint marker.

### Graphics Client Menu

#### File

The `File` menu allows the user to open a waypoint input file, save the current waypoint list to a new file, or clear the existing waypoint list.
![](images/GraphicsClient_FileOptions.png)

#### View

The `View` menu on the menu bar allows the user to choose the information displayed by the client.

![](images/GraphicsClient_ViewOptions.png)

##### Aircraft Position
This checkbox toggles the visibility of the `Aircraft Pos:` field in the top left corner of the map.

##### Aircraft Velocity
This checkbox toggles the visibility of the `Aircraft Vel:` field in the top left corner of the map.

##### Map Scale
This checkbox toggles the visibility of the `Scale:` field in the top left corner of the map.

##### Control Mode
This checkbox toggles the visibility of the `Control Mode:` field in the top left corner of the map.

##### Disabled Controls' Data
This checkbox is disabled unless Autopilot is engaged. When used, it toggles the visibility of the data from the disabled controls in the top right corner of the map.

![](images/GraphicsClient_ControlsDisabled.png)

## Dynamics Model

### General Strategy for Motion
To determine the motion of an object we generally start with Newton’s Second Law: $F = ma$ That is, force equals mass times acceleration.
This allows us to determine the rates that effect motion, that is: acceleration and velocity. Solving for acceleration, we get : $a = F/m$. 

This is the form of Newton’s Law that we generally use.
So, if we have a force acting on a mass, we can determine its acceleration. Then, we can then numerically integrate that acceleration to get velocity and then integrate the velocity to get position.

### Total Force and Acceleration

To calculate acceleration, we’ll use Newton’s Second Law as usual. The force is the total force acting on the aircraft.

![](images/Equation_1.png)

The total force on our aircraft is the sum of:

* the thrust force produced by the aircraft’s engine and
* the drag force produced by the aircraft pushing through the atmosphere.
* the turning force, dictated by gravity and the desired turning rate.

![](images/Equation_2.png)

If we affix a “Body” coordinate system to the aircraft as shown, then the thrust force will act in the +x direction, and the drag force in the –x direction. That is, thrust and drag that point in opposite directions.

![](images/BodyCoordinates.png)

### Drag Force

The magnitude of the drag force is proportional to the square of the aircraft’s speed. We’ll call our constant of proportionality $K_{drag}$ .

![](images/Equation_3.png)

By “speed”, we mean the magnitude of the velocity :

![](images/Equation_4.png)

### Thrust Force 

The magnitude of the thrust force produced by our engine can be anywhere between 0 and some value that we specify as the maximum ( $thrust_{MAX}$ ). For our aircraft. 

We said in the beginning that we want to control our aircraft by setting a “desired speed”. The amount of thrust produced obviously determines the speed.
So, how much thrust do we need to reach and maintain a desired speed?

* To increase speed, we must accelerate, in which case magnitude of the thrust must be greater than that of the drag.
* To maintain the same speed acceleration must be zero, in which case the thrust and drag magnitudes must be equal.
* To decrease speed, we must decelerate, in which case thrust magnitude must be less than that of drag.

Therefore, the thrust we require to reach and maintain a desired speed must be the same magnitude as the drag force at that desired speed.

![](images/Equation_5.png)

If our engine can produce the desired thrust, then we can set the throttle to produce that thrust ( $thrust_{actual}$ ) and we’ll speed up to that speed. But what if we can’t? The most thrust we can produce is $thrust_{MAX}$ , when we’re at full throttle.
So, we must limit $thrust_{actual}$ to that which our engine can actually produce :

![](images/Equation_6.png)
![](images/Equation_7.png)

So, the magnitude of the thrust force vector is $thrust_{actual}$ , and the direction is [1,0] in Body coordinates. 

### Aircraft Heading

If the aircraft is moving directly north, $\psi = 0$ . Directly west is $\pi/2$ . Directly east is $-\pi/2$ .

![](images/AircraftHeading.png)

### Heading Control

To control our heading, we’d like to calculate a heading rate that is proportional to the difference between our current heading and our desired heading. 

![](images/HeadingControl.png)

![](images/Equation_8.png)

Notice (in the figure) that $\psi_{diff}$ is in the direction that we want to turn. But, what if ( $\left| \psi_{diff} \right| > \pi$ )? That is, it’s greater than 180 degrees?

Do you ever have to turn more than 𝜋 radians (180 degrees) to face any direction? No, you don’t.

Let’s now define $\pis_{error}$ as the actual direction of turning.

![](images/Equation_9.png)

This is how we will calculate our desired heading rate:

![](images/Equation_10.png)

That is, our desired heading rate is proportional to the amount and direction that we need to turn.

$G_{\psi}$ is a proportionality constant.

### Turning Force

The turning force we needed to turn the desired rate is :

![](images/Equation_11.png)

Like our desired speed, just we want something doesn’t mean that the aircraft is capable of it. In our case we need to limit the magnitude of our force to 1 G ( force of gravity), that is 9.8 m/s2 * m. So if our mass is 5000 kg, we need to limit our turning force to between -49000 .. 49000 Newtons.

But we need that total force in world coordinates so we can move our aircraft around in the world.
So, if our heading of our aircraft is 𝜓, what is our direction vector expressed in world coordinates? 

![](images/Equation_12.png)
