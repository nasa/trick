# Double Compound Pendulum 


![Double Compound Pendulum Sketch](images/Picture1_ Double Compound Pendulum Sketch.png) 


A double compound pendulum is a mechanical system consisting of two bodies, one which is attached to a fixed base on one end and the other which is attached to the loose end of the first. “Compound” refers to the fact that the mass of the system is not concentrated completely at one point. These pendulums are both free to rotate about their attachment point, provided they maintain the above constraints. 


## Simplifications 

SIM_DoubleCompoundPendulum will neglect the effects of friction and air resistance. We present a classic example of a chaotic system, as slight variations in initial values can cause drastic differences and unpredictable behavior long term. 

The purpose of this sim is to model a double compound pendulum’s movement and at any given point calculate: 


1.  Angular momentum 

2.  Potential energy 

3.  Kinetic energy 

4.  Total energy 

In this simulation, you can set the initial conditions to what you’d like, and the equations within the sim will simulate how the simulation will behave given the starting conditions. 

## Changing Inputs to the Simulation 

Go to the input file from the SIM_DoubleCompoundPendulum directory 


    vi RUN_DoubleCompoundPendulum/input.py/ 


In this file is where you can change the starting angle of the system, the mass values, the lengths of the masses, and the simulation run time.



## Building and Running the Simulation 

In order to build a Trick simulation, the Trick-specific command **`trick-CP`** must be run to begin compilation process. **`trick-CP`** is located in the Trick `bin` directory. Run the **`trick-CP`** command in the SIM_DoubleCompoundPendulum directory. Upon successful compulation, the following message will be displayed in the terminal: 


    === Simulation make complete === 
To run, type in the terminal while in the SIM_DoubleCompoundPendulum directory: 

    %S_main_*.exe RUN_test/input.py 


## Kinematics of the System

To  the motion of this pendulum, we start by getting the kinematic equations associated with it. The following equations present the position in the x and y directions of the center of mass of each mass on the pendulum. 


![Positions in X and Y of mass 1](images\Picture2_First Set of Position Equations.png) 

![Positions in X and Y of mass 2](images\Picture3_Second Set of Position Equations.png) 


Once you have these values, you can differentiate them in order to derive the velocity of each pendulum: 


![Velocities in X and Y of mass 1](\images\Picture4_First set of Velocity Equations.png) 

![Velocities in X and Y of mass 1](\images\Picture5_Second set of Velocity Equations.png) 


Finally, you can use these values to derive the potential and kinetic energy of the system. 

### Energy in the System 

#### Potential Energy 

The general equation for potential energy: 


![Potential Energy General Equation](images\Picture6_General Potential Energy Equation.png) 


Can be applied to our simulation by just plugging in the values we have for the y components of position. 


![Potential Energy Equation with Variables Plugged in](images\Picture7_Potential Energy with variables plugged in.png) 


For the purpose of simplicity in coding, we included 2 variables to make the potential energy equation less hairy. 


Hence: 

![Mu1 and Mu2](images\Picture8_ Mu1 and Mu2.png) 

![Final Potential Energy Equation](images\Picture9_Final potential energy Equation) 


#### Kinetic Energy 

When deriving kinetic energy there is both translational and rotational energy, and the equation must account for both. Hence the general equation for kinetic energy is as follows. 


![Kinetic Energy General Equation](\images for Double Compound Pendulum README\Picture10_General Kinetic Energy Equation.png) 


If we plug in the position, velocity, and inertia for a slim rod where they belong in this equation, we arrive at an equation for kinetic energy as follows. 


![Kinetic Energy Equation with Variables Plugged in](\images\Picture11_Kinetic Energy with variables plugged in.png) 


Using the following trigonometric identities as well as variables to simplify the equation 


![Trigonometric Identities](\images\Picture12_Trigonometric Identities.png) 

![J1, J2, J3 for kinetic energy](\imagesPicture13_J1 and J2 and J3.png) 

We arrive at the equation used in the simulation. 

![Final Kinetic Energy Equation](\images\Picture14_Final kinetic Energy Equation.png) 

## Angular Momentum 
Angular Momentum is equal to inertia times the angular velocity of an object. As such, our equation for inertia comes out to:
![Angular Momentum Equation](\images\Picture15_AngularMomentumFormula.png)

## Results with Default Parameterization
![Graphs of Double Pendulum with Default Values](\images\Picture16_Graphs)

