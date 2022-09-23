# SAIntegrator Tutorial

The approach of this tutorial is to explain how to use the SAIntegrator library using the examples in the ```examples/``` directory. 

**There is no need to recreate the source code in this tutorial.** It already exists in the examples. So, read the explanations below, and then go play with the simulations in the
**examples/** directory, to learn how to use the SAIntegrator library. Enjoy!


## Contents
1. [Introduction](#Introduction)
1. [Basic Numerical Integration](#BasicNumericalIntegration)
2. [RootFinding (DynamicEvents)](#RootFinding_DynamicEvents)
3. [Using Euler Cromer](#UsingEulerCromer)
4. [Not Just Integrating Over Time](#NotJustIntegratingOverTime)
	* [Definite Integral Example](#DefiniteIntegralExample)
	* [Double Integral Example](#DoubleIntegralExample)
5. [Adaptive Step Size Integration](#AdaptiveStepSizeIntegration)

<a id= Introduction></a>
## Introduction

SAIntegrator is a C++ numerical integration library, that is independent of any other framework.
It can be used with Trick, or completely independent of it. It provides:

* A variety of Runge-Kutta algorithms each of which supports dynamic events.
* Euler-Cromer for oscillatory systems like springs and orbits.
* Runge-Kutta Fehlberg 4(5), a fourth order adaptive step-size algorithm. 
* Numerous instructive example simulation programs.

### A Tour of the SAIntegrator Directory

| File        | Purpose|
|-------------|--------|
| README.md   | Markdown version of the User's Manual. |
| Tutorial.md | Markdown version of the tutorial.|
| examples/   | Examples programs that demonstrate how to use the library. |
| images/     | Image files for the documentation.|
| include/    | Library header files. ```SAIntegrator.hh``` is the only one you'll need to include in your program.|
| makefile    | Builds the library and runs the unit-test suite.|
| src/        | Library src files.
| unittest/   | Unit-test programs.|

### Building the SAIntegrator Library
In the SAIntegrator directory, run ```make``` to build the library ```lib/libSAInteg.a```, and run the unit-test programs.

<a id=BasicNumericalIntegration></a>
## Basic Numerical Integration

This section demonstrates the basics of SAIntegrator using the program in
[examples/Cannonball](examples/CannonBall/README.md), which simulates a cannon
ball trajectory.

<a id=listing-Cannonball.cpp></a>
**Listing - Cannonball.cpp**

```C++
#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

struct Cannon {
    double pos[2];
    double vel[2];
    Cannon(double px, double py, double vx, double vy);
};
Cannon::Cannon(double px, double py, double vx, double vy) {
    pos[0] = px;  pos[1] = py;
    vel[0] = vx;  vel[1] = vy;
}
void CSV_header() {
    printf ("t, cannon.pos[0], cannon.pos[1], cannon.vel[0], cannon.vel[1]\n");
}
void CSV_state( double t, Cannon& cannon) {
    printf ("%5.3f, %5.10f, %5.10f, %5.10f, %5.10f\n",
    t, cannon.pos[0], cannon.pos[1], cannon.vel[0], cannon.vel[1]);
}
void calc_derivs( double t, double state[], double derivs[], void* udata) {
    derivs[0] = state[2];
    derivs[1] = state[3];
    derivs[2] =   0.0;
    derivs[3] = -9.81;
}
int main ( int argc, char* argv[]) {
    const double muzzle_speed = 50; // m/s
    const double muzzle_angle = 30; // degrees
    double vx0 = muzzle_speed * cos(muzzle_angle * M_PI / 180.0);
    double vy0 = muzzle_speed * sin(muzzle_angle * M_PI / 180.0);
    Cannon cannon(0.0, 0.0, vx0, vy0);

    double* state_var_p[4] = { &(cannon.pos[0]), &(cannon.pos[1]),
                               &(cannon.vel[0]), &(cannon.vel[1]) };
    double dt = 0.01;
    SA::RK2Integrator integ(dt, 4, state_var_p, calc_derivs, NULL);

    double t  = 0.0;
    CSV_header();
    CSV_state( t, cannon);
    while (t < 5.1) {
        integ.integrate();
        t = integ.getIndyVar();
        CSV_state( t, cannon);
    }
}
```

In [Cannonball.cpp](#listing-Cannonball.cpp) , ```struct Cannon``` represents our cannon ball state.

Functions ```CSV_header()``` and ```CSV_state``` produce comma separated value (CSV) output for our sim.

#### Integrator Constructor

In **main** the following line constructs the Integrator that will propogate our simulation state :

```C++
SA::RK2Integrator integ(dt, 4, state_var_p, calc_derivs, NULL);

``` 
It creates an instance of ```SA::RK2Integrator``` called ```integ```.

We could just as easily have used ```SA::EulerIntegrator```, ```SA::HeunsMethod```, ```SA::RK4Integrator```, or ```SA::RK3_8Integrator```, as their [constructor](#RK2IntegratorConstructor2) interfaces are identical.

The first argument of this constructor specifies the integration step-size. We will be integrating in 0.01 second steps.

The second argument specifies the state-size, the number of variables that we will be integrating per step. In this case we'll be integrating two position variables, and two velocity variables, so our state-size is 4. 

The third specifies which variables comprise the state, to be updated by the Integrator. Here, we want to update the variables: cannon.pos[0], cannon.pos[1], cannon.vel[0]), and cannon.vel[1].

The fourth argument of the constructor is a pointer to a function that is responsible for generating state derivatives. Its type is as follows:

```C++
typedef void (*DerivsFunc)( double x, double state[], double derivs[], void* udata);
```

It's described in more detail [here](#typedef-DerivsFunc).

The final argument specifies a pointer to user-data which is passed-through to the derivative function and others. We'll demonstrate it later in this tutorial. For now we'll just pass NULL.

The array ```double* state_var_p[4]```, specifies the state variables to be updated by Integrator.  

<a id=DerivsFunc></a>
#### DerivsFunc

The cannon ball simulation implements the following ```DerivsFunc```:

```C++
void calc_derivs( double t, double state[], double derivs[], void* udata) {
    derivs[0] = state[2];
    derivs[1] = state[3];
    derivs[2] =   0.0;
    derivs[3] = -9.81;
}
```
The Integrator calls ```calc_derivs``` function to generate the state derivatives, that is velocity, and acceleration. 

First argument passed to this function is the current value of the independent variable. In this case, it represents time. 

The second argument passed in is the current state array. It's length is the Integrator's state-size. The order of state values matches the order specified in ```state_var_p``` in the constructor.

The third argument, ```derivs```, is an array of (also of length state-size), which ```calc_derivs``` is responsible for populating.

For clarity here: state[0] and state[1] represent the cannonball position. state[2] and state[3] represent the cannonball velocity. derivs[0] and derivs[1] represents the time derivative of position, i.e., velocity. derivs[2] and derivs[3] represents the time derivative of velocity, i.e., acceleration.

#### The Integration Loop

The while loop, at the bottom of [Cannonball.cpp](#listing-Cannonball.cpp) is where the integration happens. 

```C++
    while (t < 5.1) {
        integ.integrate();
        t = integ.getIndyVar();
        CSV_state( t, cannon);
    }
```


```integ.integrate();``` does the following:

1. Loads the values of the variables that we specified in ```state_var_p``` into the integrator.
2. Steps forward by ```dt```, integrating the state-derivatives into the state, and finally
3. Unloads the result back into the variables that we specified in ```state_var_p```.

#### Getting and Setting The Independent Variable

```t = integ.getIndyVar();``` gets the time from the Integrator. So, why isn't this method called **getTime()**? Because we don't just have to integrate over time. We're integrating over the independent variable, which in the case of the Cannonball simulation happens to be time.

```integ.setIndyVar( double )``` sets the independent variable. So if we wanted to start our sim at t=4.0 seconds then we could replace ```double t = 0.0;``` with:

```C++
double t  = 4.0;
integ.setIndyVar(t);
```

We also might want to change ```while (t < 5.1)``` to ```while (t < 9.1)```.

#### [How to Run The CannonBall Example](examples/CannonBall/README.md)

<a id=RootFinding_DynamicEvents></a>
## RootFinding (Dynamic Events)

This section demonstrates how to use a RootFinder with our integrator, using the program in [examples/BouncyCannonball](examples/BouncyCannonBall/README.md). This example simulates a cannon ball that impacts the ground (defined as pos[1] = 0.0), and bounces. The BouncyCannonball example is the same as the Cannonball example, with some additional code.

The first code addition is to add the following line immediately after the instanciation of ```integ```.

```C++
integ.add_Rootfinder(1.0e-10, Negative, &impact);
```
The ```add_Rootfinder``` method tells ```integ``` to look for roots. 

```add_Rootfinder``` is available for: ```SA::RK2Integrator```, ```SA::EulerIntegrator```, ```SA::HeunsMethod```, ```SA::RK4Integrator```, and ```SA::RK3_8Integrator```.

The first argument is the tolerance; the maximum error we'll accept. The error is the difference between some element of the current integration state (of our chosing), and the root of that element. The root is the value of the independent variable, where the particular state-element of interest (or function of that state-element) is zero. Specifically, for the BouncyCannonball example, a root is a value of ```t```, when the ball impacts the ground, where ```pos[1] == 0.0```.

The second argument is the slope constraint. This can have one three values:

1. ```Negative``` - the slope of the element of interest must be negative.
2. ```Unconstrained``` -  no slope constraint.
3. ```Positive``` - the slope of the element of interest must be positive.

The third argument is a pointer to a function responsible for estimating the error. It's type is:

```C++
typedef double (*RootErrorFunc)( double x, double state[], RootFinder* root_finder, void* udata);
```

The RootErrorFunc for BouncyCannonball is defined as:

```C++
double impact( double t, double state[], RootFinder* root_finder, void* udata) {
    double root_error = root_finder->find_roots(t, state[1]);
    if (root_error == 0.0) {
        root_finder->init();
        state[2] =  0.9 * state[2];
        state[3] = -0.9 * state[3];
    }
    return (root_error);
}
```
The [```find_roots```](#function-find_roots) method of the ```RootFinder``` object will determine the estimated correction in ```t``` to get ```state[1]``` to zero. Remember that ```state[1]``` is the value of cannon.pos[1], the cannon ball height above the ground.

If the error == 0.0, then ```state[1]``` is zero, meaning that our cannon ball has contacted the ground at time ```t```. 

The first thing to do after finding the root is to reset the rootfinder, by calling ```root_finder->init();```. This allows the rootfinder to find more
roots as time progresses.

Then we make our impact result happen. Here, we want the cannon ball to bounce. So we flip the sign of the vertical velocity (```state[3]```), and keep the sign of the horizontal velocity as-is. In BouncyCannonBall we also make the cannonball lose a little bit of energy, so we multiply our velocity vector by 0.9.

The last difference between CannonBall and BouncyCannonBall is the change in the
duration of the simulation (So the bouncy ball can bounce several times).

So, the line ```while (t < 5.1) { ``` is changed to

```while (t < 20.0) { ```

#### [How to Run The BouncyCannonBall Example](examples/BouncyCannonBall/README.md)

<a id=UsingEulerCromer></a>
## Using Euler-Cromer

This section demonstrates the ```EulerCromerIntegrator``` class using 
the program in [examples/MassSpringDamper](examples/MassSpringDamper/README.md)

The EulerCromer algorithm is not a general-purpose integration algorithm like Runge-Kutta algorithms. It is integration algorithm often used in oscillatory systems like mass-spring-damper systems, and orbital systems because it conserves energy much better than Runge-Kutta.

**Listing - MassSpringDamper.cpp**

```C++
#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

struct MassSpringDamper {
    double pos;
    double vel;
    double k;     // Spring constant
    double c;     // Damping constant
    double mass;
    double extForce;
    MassSpringDamper(double k, double c, double m);
};
MassSpringDamper::MassSpringDamper( double k, double c, double m)
: pos(0.0), vel(0.0), k(k), c(c), mass(m), extForce(0.0) {}

struct ImpulseGenerator {
    double period;
    double duration;
    double force;
    ImpulseGenerator(double period, double duration, double force);
    double impulse(double time);
};
ImpulseGenerator::ImpulseGenerator(double period, double duration, double force)
: period(period), duration(duration), force(force) {}
double ImpulseGenerator::impulse(double time) {
    double tt = fmod(time, period);
    if ((tt >= 0.0) && (tt < duration)) return force;
    return 0.0;
}
void print_header() {
    printf ("time, msd.pos, msd.vel\n");
}
void print_state( double t, MassSpringDamper& msd ) {
    printf ("%10.10f, %10.10f, %10.10f\n", t, msd.pos, msd.vel);
}
void acceleration( double t, double x[], double v[], double g_out[], void* udata) {
    MassSpringDamper* msd = (MassSpringDamper*)udata;
    g_out[0] = -(msd->k/msd->mass) * x[0]
               -(msd->c/msd->mass) * v[0]
               + msd->extForce/msd->mass;
}
#define N_DIMENSIONS 1
int main ( int argc, char* argv[]) {
    const double mass = 1.0;            // kg
    const double frequency = 2;         // Hz
    const double dampingConstant = 5.0; // kg/s = Ns/m
    double springConstant = 4.0 * M_PI * M_PI * frequency * frequency * mass; // kg/s^2 = N/m
    MassSpringDamper msd(springConstant, dampingConstant, mass);
    ImpulseGenerator ImpGen(5.0, 0.005, 500.0); // 500 Newtons for 0.005 seconds every 5 seconds.
    double time = 0.0;
    double* x_p[N_DIMENSIONS] = { &msd.pos };
    double* v_p[N_DIMENSIONS] = { &msd.vel };
    double dt = 0.001;
    print_header();
    print_state(time, msd);
    SA::EulerCromerIntegrator integ(dt, N_DIMENSIONS, x_p, v_p, acceleration, &msd);
    while (time < 10) {
        msd.extForce = ImpGen.impulse(time);
        integ.integrate();
        time = integ.getIndyVar();
        print_state(time, msd);
    }
}
```

```struct MassSpringDamper``` specifies the attributes of the [mass-spring-damper](https://en.wikipedia.org/wiki/Mass-spring-damper_model) system we want to simulate.

```struct ImpulseGenerator``` specifies a forcing function for our system, primarily to make
this simulation a little more interesting.

We've seen the ```print_header```, and ```print_state``` functions before. They're for generating CSV output for the simulation.

The ```acceleration``` function generates derivatives for the ```EulerCromerIntegrator```. It's of type [```Derivs2Func```](#typedef-Derivs2Func), a little different from the derivative generator function for Runge-Kutta Integrators.

```void acceleration( double t, double x[], double v[], double g_out[], void* udata)```

In ```main```, the following line [constructs](#EulerCromerIntegratorConstructor) our integrator:

```SA::EulerCromerIntegrator integ(dt, N_DIMENSIONS, x_p, v_p, acceleration, &msd);```

The first argument (```dt```) specifies the integration step-size. We will be integrating in 0.001 second steps.

The second argument specifies the size of the position, and velocity vectors. So if our system model 3-dimensional, then this would be 3. This simulation is only one-dimensional so it's value is 1.

The third argument, ```x_p``` is a pointer to an array (of length one) of pointers to variables from which we load() and to which we unload() the position vector .

The fourth argument, ```v_p``` is a pointer to an array of pointers to variables from which we load() and to which we unload() the velcity vector.

The fifth argument of the constructor is a pointer to a function that is responsible for generating state derivatives. Its type is as follows:

<a id=typedef-Derivs2Func></a>
### typedef Derivs2Func

```
typedef void (*Derivs2Func)( double t, double x[], double v[], double derivs[], void* udata);
```

The sixth, and final argument is the user_data. In this case we're passing the ```msd```, i.e.,(```MassSpringDamper```) object.

#### [Running the MassSpringDamper Example](examples/MassSpringDamper/README.md).

<a id=NotJustIntegratingOverTime></a>
## Not Just Integrating Over Time

This section demonstrates using SAIntegrator to evaluate a definite integral. using the program in [examples/DefiniteIntegral](examples/DefiniteIntegral/README.md).

<a id=DefiniteIntegralExample></a>
### A Definite Integral Example

Given the coefficients of a third order polynomial, and limits of integration, evaluate the integral :

![Def Integral of General 3rd Order Polynomial](examples/DefiniteIntegral/images/Eq1.png)

This example specifically evaluates:

![Def Integral of Specific 3rd Order Polynomial](examples/DefiniteIntegral/images/Eq2.png)

<a id=listing-DefiniteIntegral.cpp></a>
### Listing - DefiniteIntegral.cpp

```C++
#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

struct DefiniteIntegral {
    double lower_limit;
    double upper_limit;
    double result;
    double Coeff[4];
    SA::Integrator* integrator;
    double evaluate ();
};
double DefiniteIntegral::evaluate () {
    result = 0.0;
    integrator->setIndyVar(lower_limit);
    while(integrator->getIndyVar() < upper_limit){
        integrator->integrate();
    }
    return result;
}
void Order3Polynomial( double x, double state[], double derivs[], void* udata) {
    DefiniteIntegral& DI = *(DefiniteIntegral*)udata;
    double x2 = x*x;
    double x3 = x*x2;
    derivs[0] = DI.Coeff[3] * x3 +
                DI.Coeff[2] * x2 +
                DI.Coeff[1] * x +
                DI.Coeff[0];
}
int main (int argc, char* argv[]) {
    unsigned int NSteps = 20;
    DefiniteIntegral DI;
    DI.Coeff[0] =  7.0;
    DI.Coeff[1] = -5.0;
    DI.Coeff[2] =  1.0;
    DI.Coeff[3] =  2.0;
    DI.lower_limit =  0.0;
    DI.upper_limit =  2.0;
    double dx = (DI.upper_limit - DI.lower_limit) / NSteps;
    double* state[1] = { &DI.result };
    DI.integrator = new SA::RK4Integrator(dx, 1, state, Order3Polynomial, &DI);
    double result = DI.evaluate();
    printf("Integral = %g.\n", result);
}
```

At the top of [DefiniteIntegral.cpp](#listing-DefiniteIntegral.cpp) , ```struct DefiniteIntegral ``` specifies everything relevant to the task. The ```evaluate``` method evaluates the integral from ```lower_limit``` to ```upper_limit```, and places the result in ```result```. The ```Coeff``` array contains the polynomial coefficients. ```integrator``` points to the SA::Integrator we intend to use.


```Order3Polynomial``` is our derivative function. Note we pass in the ```DefiniteIntegral``` object via ```udata```, so we have access to the coefficients.

```main``` sets up a ```DefiniteIntegral``` object and calls evaluate() the get the answer.

Note how ```dx``` is determined:

```
double dx = (DI.upper_limit - DI.lower_limit) / NSteps;
```
This is to ensure the accuracy of the integration range. 


#### [Running the DefiniteIntegral Example](examples/DefiniteIntegral/README.md)

<a id=DoubleIntegralExample></a>
### A Double Integral Example

This example demonstrates nested integration using the program in [examples/DoubleIntegral](examples/DoubleIntegral/README.md).

<a id=listing-DoubleIntegral.cpp></a>
### Listing - DoubleIntegral.cpp

```C++
#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

struct DefiniteIntegrator {
    double ivar;
    double start;
    double end;
    double result;
    SA::Integrator* integrator;
    double evaluate ();
};
double DefiniteIntegrator::evaluate () {
    result = 0.0;
    integrator->setIndyVar(start);
    ivar = start;
    while (ivar < end) {
        integrator->integrate();
        ivar = integrator->getIndyVar();
    }
    return result;
}
struct IntegContext {
    DefiniteIntegrator XDI;
    DefiniteIntegrator YDI;
};
void deriv_y( double y, double state_y[], double derivs_y[], void* udata) {
    IntegContext& IC = *(IntegContext*)udata;
    derivs_y[0] = cos(IC.XDI.ivar) * cos(y);
}
void deriv_x( double x, double state_x[], double derivs_x[], void* udata) {
    IntegContext& IC = *(IntegContext*)udata;
    derivs_x[0] = IC.YDI.evaluate();
}
double doubleIntegral( double x_start, double x_end, double y_start, double y_end ) {
    IntegContext IC;
    double dx = (x_end - x_start) / 20.0;
    double dy = (y_end - y_start) / 20.0;
    double* state_x[1] = { &IC.XDI.result };
    double* state_y[1] = { &IC.YDI.result };

    IC.XDI.start = x_start;
    IC.XDI.end   = x_end;
    IC.XDI.integrator = new SA::RK4Integrator(dx, 1, state_x, deriv_x, &IC);

    IC.YDI.start = y_start;
    IC.YDI.end   = y_end;
    IC.YDI.integrator = new SA::RK4Integrator(dy, 1, state_y, deriv_y, &IC);

    return (IC.XDI.evaluate());
}
int main ( int argc, char* argv[]) {
    double vol = doubleIntegral( -M_PI/2.0, M_PI/2.0, -M_PI/2.0, M_PI/2.0 );
    printf("Volume = %g.\n", vol);
}
```
In the [DefiniteIntegral](examples/DefiniteIntegral/README.md) example, we use
```struct DefiniteIntegrator``` to specify everything necessary to evaluate a single definite integral. But, a nested integrator also needs access to the independent variables of the integrators that "enclose" it. So, in the  DoubleIntegral example we need to modify ```struct DefiniteIntegrator```. First we expose the independent variable (```ivar```) and keep it updated in the ```evaluate``` method. Then we combine, or "stack" two DefiniteIntegrator's in ```struct IntegContext``` to be passed in user_data, so "inner" integrators have access to "outer" integrator's independent variables.
1. 
#### [Running the Double Integral Example](examples/DoubleIntegral/README.md).

<a id=AdaptiveStepSizeIntegration></a>
## Adaptive Step Size Integration

This section demonstrates the ```RKF45Integrator``` class using 
the program in [examples/AsteroidFlyBy](examples/AsteroidFlyBy/README.md).

The RKF45Integrator is an adaptive step-size integrator. It adapts the
integration step-size to maintain a specified accuracy. If a particular step-size 
doesn't produce the needed accuracy then the step-size is reduced and the integration step is performed again. If the needed accuracy is being produced then the step-size can be increased. There is some over-head in the extra calculations, that estimate the local-error. But, this can be more than made up for by the fact that the step-size is small **only** when necessary.

The listing below is a simulation of a relatively small asteroid flying past the Earth. As the asteroid approaches, and the acceleration of gravity gets stronger, the ```RKF45Integrator``` gradually reduces the step-size to maintain the required accuracy. As the asteroid retreats, the step-size is gradually increased, back to its normal, maximum value.  

<a id=listing-Flyby.cpp></a>
### Listing - Flyby.cpp
```C++
#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

#define GRAVITATIONAL_CONSTANT 6.674e-11
#define EARTH_MASS 5.9723e24
#define EARTH_RADIUS 6367500.0

struct Flyby {
    double pos[2];
    double vel[2];
    double planet_mass;
    Flyby(double px, double py, double vx, double vy, double m);
};
Flyby::Flyby(double px, double py, double vx, double vy, double m) {
    pos[0] = px;
    pos[1] = py;
    vel[0] = vx;
    vel[1] = vy;
    planet_mass = m;
}
void print_header() {
    printf ("time, dt, flyby.pos[0], flyby.pos[1], flyby.vel[0], flyby.vel[1]\n");
}
void print_state( double t, double dt, Flyby& flyby ) {
    printf ("%10.10f, %10.10f, %10.10f, %10.10f, %10.10f, %10.10f\n",
             t, dt, flyby.pos[0], flyby.pos[1], flyby.vel[0], flyby.vel[1]);
}
void gravity( double t, double* state, double derivs[], void* udata) {
    Flyby* flyby = (Flyby*)udata;
    double d = sqrt( state[0]*state[0] + state[1]*state[1]);
    derivs[0] = state[2];
    derivs[1] = state[3];
    derivs[2] = -state[0] * GRAVITATIONAL_CONSTANT * flyby->planet_mass / (d*d*d);
    derivs[3] = -state[1] * GRAVITATIONAL_CONSTANT * flyby->planet_mass / (d*d*d);
}
int main ( int argc, char* argv[]) {

    double sim_duration = 25000.0; // s
    double dt = 60.0; // s
    double epsilon = 0.000000001;
    Flyby flyby(-20.0 * EARTH_RADIUS, 2.0 * EARTH_RADIUS, 10000.0 , 0.0, EARTH_MASS);
    double* state_p[4] = { &flyby.pos[0], &flyby.pos[1], &flyby.vel[0], &flyby.vel[1] };

    double time = 0.0; // s
    print_header();
    print_state(time, dt, flyby);

    SA::RKF45Integrator integ(epsilon, dt, 4, state_p, gravity, &flyby);
    while (time < sim_duration) {
        integ.integrate();
        double last_h = integ.getLastStepSize();
        time = integ.getIndyVar();

        double r = sqrt( flyby.pos[0]*flyby.pos[0] + flyby.pos[1]*flyby.pos[1]);
        if (r < EARTH_RADIUS) { printf("Collision\n"); }
        print_state(time, last_h, flyby);
    }
}
```

 ```struct Flyby ``` specifies the position of an asteroid, and the mass of the planet with which it's interacting. 

The ```print_header```, and ```print_state``` functions generate CSV output for the simulation.

The  function ```gravity```, of type [```DerivsFunc```](#typedef-DerivsFunc) generates derivatives for the ```RKF45Integrator```. 

In ```main```, the following line [constructs](#RKF45IntegratorConstructor3``) our integrator:

```SA::RKF45Integrator integ(epsilon, dt, 4, state_p, gravity, &flyby);```

The first argument (```epsilon```) is unique to adaptive step-size integration. It specifies the allowable "local error", that is: the error per step. The ```RKF45Integrator``` automatically adjusts the step-size needed to achieve this accuracy. Local error is calculated for each state element. The largest of the element errors guides the step-size adaptation of the algorithm.

The second argument (```dt```) specifies the **maximum** integration step-size. Notice the difference between this, and non-adaptive step size integrators. Here, the actual step size is determined by the Runge-Kutta-Fehlberg algorithm. The method ```getLastStepSize()``` returns this actual step-size. It may be smaller than, but will not be larger than ```dt```.

In this simulation, ```dt``` is 60 seconds, but as the asteroid approaches the planet, and the acceleration of gravity increases, the Runge-Kutta-Fehlberg algorithm gradually reduces the step size down to ~ 3 seconds at closest approach to maintain the specified local accuracy.

The third argument specifies the state-size, in this case: 4. 

The fourth argument specifies the list of variables comprising the state: ```flyby.pos[0], flyby.pos[1], flyby.vel[0]), and flyby.vel[1]```.

The fifth argument is a pointer to the derivatives generator function, ```gravity``` of type [(DerivsFunc)](#typedef-DerivsFunc).

The sixth argument is a pointer to the Flyby object.

#### [Running the AsteroidFlyBy Example](examples/AsteroidFlyBy/README.md).

## The End


# Appendix

<a id=RK2IntegratorConstructor2></a>
## RK2Integrator Constructor (2)

### Description

This is the second of four available RK2Integrator constructors. This constructor interface is common to all of the Integrators derived from class ```SA::FirstOrderODEIntegrator```: ```SA::EulerIntegrator```, ```SA::HeunsMethod```, ```SA::RK2Integrator```, ```SA::RK4Integrator```, and ```SA::RK3_8Integrator```. 

```C++
RK2Integrator( double h,
               int N,
               double* in_out_vars[],
               DerivsFunc func,
               void* user_data); 
```
where:

|Parameter    |Type         |Description|
|-------------|-------------|-----------|
|h            |```double``` |Default integration step-size. |
|N            |```int```    |Number of state variables to be integrated|
|in\_out\_vars      |```double*```|Array of pointers to the state variables from which we ```load()```, and to which we ```unload()``` the integrator state |
| derivs_func |[```DerivsFunc```](#typedef-DerivsFunc)| Function thats generates the function (the derivatives) to be integrated. |
|user_data    |```void*```  | A pointer to user defined data that will be passed to a DerivsFunc when called by the Integrator. |

For more information, check out the SAIntegrator User's Guide, or even ```SAIntegrator.hh```.


<a id=RKF45IntegratorConstructor3></a>
## RKF45Integrator Constructor

### Description

This is the third of four available RKF45Integrator constructors.

```C++
RKF45Integrator( double epsilon,
                 double h,
                 unsigned int N,
                 double* in_out_vars[],
                 DerivsFunc derivs_func,
                 void* udata);
```
where:

|Parameter    |Type         |Description|
|-------------|-------------|-----------|
|epsilon      |```double``` | Specifies the maximum allowable error per step.|
|h            |```double``` | Default integration step-size. This is the maximum allowable step-size. |
|N            |```int```    |Number of state variables to be integrated|
|in\_out\_vars      |```double*```|Array of pointers to the state variables from which we ```load()```, and to which we ```unload()``` the integrator state |
| derivs_func |[```DerivsFunc```](#typedef-DerivsFunc)| Function thats generates the function (the derivatives) to be integrated. |
|user_data    |```void*```  | A pointer to user defined data that will be passed to a DerivsFunc when called by the Integrator. |

For more information, check out the SAIntegrator User's Guide, or even ```SAIntegrator.hh```.



<a id=typedef-DerivsFunc></a>
## typedef DerivsFunc

### Description
This typedef defines a type of C/C++ function whose purpose is to populate
a state derivative array.

```C++
typedef void (*DerivsFunc)( double x, double state[], double derivs[], void* udata);
```
where:

|Parameter|Type         |Direction|Description|
|---------|-------------|---------|-----------|
|x        |```double``` |IN       |Independent variable.|
|state    |```double*```|IN       |Array of state variable values.|
|derivs   |```double*```|OUT      |Array into which derivatives are to be returned.|
|udata    |```void*```  |IN       |Pointer to user_data.|

<a id=EulerCromerIntegratorConstructor></a>
## EulerCromerIntegrator Constructor
|Parameter  |Type         |Description|
|-----------|-------------|-----------------------|
| dt        |```double``` |Default time step value. Sets Integrator::default_h. |
| N         |```int```    |Number of dimensions in position, velocity, and acceleration vectors. Typically 1,2, or 3.|
| xp        |```double*```|Array of pointers to variables from which we ```load()``` and to which we ```unload()``` the position values .|
| vp        |```double*```|Array of pointers to variables from which we ```load()``` and to which we ```unload()``` the velocity values .|
| gfunc |[```Derivs2Func```](#typedef-Derivs2Func)| A pointer to a function that returns accelerations. |
|user_data  |```void*```  | Sets Integrator::user_data. |

<a id=typedef-Derivs2Func></a>
## typedef Derivs2Func

### Description
This typedef defines a type of C/C++ function whose purpose is to populate
an array of accelerations, given velocities and positions.

```C++
typedef void (*Derivs2Func)( double t, double x[], double v[], double a[], void* udata);
```
where:

|Parameter|Type         |Direction|Description|
|---------|-------------|---------|-----------|
|t        |```double``` |IN       |Independent variable.|
|x        |```double*```|IN       |Array of position values.|
|v        |```double*```|IN       |Array of velocity values.|
|a        |```double*```|OUT      |Array into which accelerations are to be returned.|
|udata    |```void*```  |IN       |Pointer to user_data.|


<a id=typedef-RootErrorFunc></a>
## typedef RootErrorFunc

### Description

This typedef defines a type of C/C++ function whose purpose is to specify the job of a [```RootFinder```](#class-RootFinder).

```C++
typedef double (*RootErrorFunc)( double x, double state[], RootFinder* root_finder, void* udata);
```

where:

|Parameter   |Type             |Direction|Description|
|------------|-----------------|---|--------|
|x           |```double```     |In | Independent variable |
|state       |```double*```    |In | Array of state variable values |
|root_finder |```RootFinder*```|In | Class for finding the roots of a function.|
|udata       |```void*```      |In | A pointer to user_data.|

A function of type **RootErrorFunc** should:

1. Specify a (math) function f(x) whose roots [x : f(x)=0] the RootFinder is meant to find. 
    - f(x) is usually a function of the state variables. State variables are themselves functions of x.
2. Call [```root_finder->find_roots(x, y)```](#member-find_roots), where y = f(x). If it returns 0.0, it's found a root of f(x).
3. Specify what to do as a result of finding a root.
4. Return the value returned by ```root_finder->find_roots()```.


=============
<a id=function-find_roots></a>
## RootFinder::find_roots
```double RootFinder::find_roots( double x, double f_error )```

```x```: represents the independent variable.

```f_error```: is the value of the state-element of interest, or a function of it. Note that a function of a function of x is itself a function of x.

* If no root is detected returns DBL_MAX.
* If a root is detected :
    * If the estimated error in f(x) is within tolerance return 0.0.
    * If the estimated error in f(x) is not within tolerance return an estimated correction for x.

<a id=SideNote1></a>
## Side Note 1
```integ.integrate()``` is a convenience function. It's implemented as simply:

```C++
void SA::Integrator::integrate() {
    load();
    step();
    unload();
}
```
so, 

```integ.integrate();``` could be replaced with:

```C++
    integ.load();
    integ.step();
    integ.unload();
```
with identical results.

Rather than constantly unloading and then reloading our last result, we load from One reason is  ```load_from_outState()```.
``
