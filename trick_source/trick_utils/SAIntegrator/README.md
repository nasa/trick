# Stand-Alone Integration Library

## Contents

* [Introduction](#Introduction)
* [class Integrator](#class-Integrator)
* [typedef derivsFunc](#typedef-derivsFunc)
* [class FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator)
* [class FirstOrderODEVariableStepIntegrator](#class-FirstOrderODEVariableStepIntegrator)
* [class EulerIntegrator](#class-EulerIntegrator)
* [class HeunsMethod](#class-HeunsMethod)
* [class RK2Integrator](#class-RK2Integrator)
* [class RK4Integrator](#class-RK4Integrator)
* [class RK3_8Integrator](#class-RK3_8Integrator)
* [class EulerCromerIntegrator](#class-EulerCromerIntegrator)
* [class ABM2Integrator](#class-ABM2Integrator)
* [class ABM4Integrator](#class-ABM4Integrator)

<a id=Introduction></a>
## Introduction
The Stand-Alone Integration Library can be used within a Trick simulation, or independent of it.

Some examples of using these integrators can be found in the **examples/** directory.

* [CannonBall](examples/CannonBall/README.md) uses the RK2Integrator.
* [MassSpringDamper](examples/MassSpringDamper/README.md) uses the EulerCromerIntegrator.
* [Orbit](examples/Orbit/README.md) uses the EulerCromerIntegrator.

<a id=class-Integrator></a>
## class Integrator

### Description
This class represents an **integrator**.

### Constructor

#### ```Integrator(double dt, void* user_data);```


|Parameter|Type        |Description|
|---------|------------|--------------------|
|dt       |```double```|Default time step value|
|user_data|```void*```| A pointer to user defined data that will be passed to a derivsFunc when called by the Integrator. |
### Member Functions

#### ```virtual void step();```

Increment time by the default time-step.

#### ```virtual void load() = 0;```

Load the input state.

#### ```virtual void unload() = 0;```

Load the output state.

#### ```double getTime();```
Return the integrator time.

<a id=typedef-derivsFunc></a>
## typedef derivsFunc

```
typedef void (*derivsFunc)( double t, double state[], double derivs[], void* user_data);
```
where:

|Parameter|Type|Description|
|---|---|---|
|t|```double```| time|
|state|```double*```| (input) an array of state variable values |
|derivs|```double*```| (output) an array into which derivatives are to be returned|
|user_data|```void*```| a pointer to user_data |

#### Example
```
void my_derivs( double t, double state[], double deriv[], void* udata) { ... }
```

<a id=class-FirstOrderODEIntegrator></a>
## class FirstOrderODEIntegrator
Derived from [Integrator](#class-Integrator).

### Description
This class represents an integrator for a first order [Ordinary Differential Equation (ODE)]([https://en.wikipedia.org/wiki/Ordinary_differential_equation).

### Constructor

```
FirstOrderODEIntegrator( double dt,
                         int N,
                         double* in_vars[],
                         double* out_vars[],
                         derivsFunc func,
                         void* user_data); 
```
where:

<a id=FOODEConstructorParameters></a>

|Parameter|Type|Description|
|---|---|---|
|dt|```double```|Default time step value|
|N|```int```|Number of state variables to be integrated|
|in_vars|```double*```|Array of pointers to the state variables from which we ```load()``` the integrator state (```in_vars``` and ```out_vars``` will generally point to the same array of pointers.)|
|out_vars|```double*```|Array of pointers to the state variables to which we ```unload()``` the integrator state (```in_vars``` and ```out_vars``` will generally point to the same array of pointers.)|
| derivs_func |[derivsFunc](#typedef-derivsFunc)| A function that returns |
|user_data|```void*```| A pointer to user defined data that will be passed to a derivsFunc when called by the Integrator. |

### Member Functions

#### void step( double dt )
Integrate over time step specified by **dt**.

|Parameter|Type|Description|
|---|---|---|
|dt | double| A variable time-step |

#### virtual void undo_step()

If the integrator has not already been reset then :

1. Decrement time by the last time-step dt.
2. Copy the input state back to the origin variables.
3. Set the integrator's 'reset' mode to true. 

Calling load() sets the 'reset' mode to false. 

#### void step()
Integrate over the default time-step.

#### void load()
Load the integrator's initial state from the variables specified by **in_vars**.

#### void unload();

Unload the integrator's result state to the variables specified by **out_vars**.

<a id=class-FirstOrderODEVariableStepIntegrator></a>
## class FirstOrderODEVariableStepIntegrator

### Description
This class represents an integrator for a first order ODE, that be 

### Constructor
### Member Functions
#### virtual void step( double dt );

<a id=class-EulerIntegrator></a>
## class EulerIntegrator
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

### Description
The Euler method is a first order numerical integration method. It is the simplest, explicit Runge–Kutta method.
### Constructor
```
EulerIntegrator(double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```
[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-HeunsMethod></a>
## class HeunsMethod
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).
### Description
This integrator implements 
[Heun's Method](https://en.wikipedia.org/wiki/Heun%27s_method).

### Constructor
```
HeunsMethod( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```
[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-RK2Integrator></a>
## class RK2Integrator
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).
### Description
The Runga-Kutta-2 method is a second order, explicit, numerical integration method.
### Constructor
```
RK2Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```
[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-RK4Integrator></a>
## class RK4Integrator
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).
### Description
The Runga-Kutta-4 method is a fourth order, explicit, numerical integration method.
### Constructor
```
RK4Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```
[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-RK3_8Integrator></a>
## class RK3_8Integrator
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).
### Description
The Runga-Kutta-3/8 method is a fourth order, explicit, numerical integration method.
### Constructor
```
RK3_8Integrator( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```
[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-ABM2Integrator></a>
## class ABM2Integrator
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).
### Description

The ABM2Integrator implements the second-order Adams-Bashforth-Moulton predictor/corrector method. Adams methods maintain a history of derivatives rather than calculating intermediate values like Runga-Kutta methods.

### Constructor
```
ABM2Integrator ( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```
[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-ABM4Integrator></a>
## class ABM4Integrator
Derived from [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

### Description
The ABM2Integrator implements the second-order Adams-Bashforth-Moulton predictor/corrector method. Adams methods maintain a history of derivatives rather than calculating intermediate values like Runga-Kutta methods.

### Constructor
```
ABM4Integrator ( double dt, int N, double* in_vars[], double* out_vars[], derivsFunc func, void* user_data)
```

[Constructor Parameters](#FOODEConstructorParameters) are those [FirstOrderODEIntegrator](#class-FirstOrderODEIntegrator).

<a id=class-EulerCromerIntegrator></a>
## EulerCromerIntegrator
Derived from [class-Integrator](#class-Integrator).

### Description
EulerCromer is integration method that conserves energy in oscillatory systems better than Runge-Kutta. So, it's good for mass-spring-damper systems, and orbital systems.

### Constructor
```
SemiImplicitEuler(double dt, int N, double* xp[], double* vp[], derivsFunc gfunc, derivsFunc ffunc, void* user_data)
```

|Parameter|Type         |Description|
|-----------|-------------|-----------------------|
| dt        |```double``` |Default time step value|
| N         |```int```    |Number of state variables to be integrated|
| xp        |```double*```|Array of pointers to the variables from which we ```load()``` and to which we ```unload()``` the integrator's position values .|
| vp        |```double*```|Array of pointers to the variables from which we ```load()``` and to which we ```unload()``` the integrator's velocity values .|
| gfunc |[derivsFunc](#typedef-derivsFunc)| A function that returns acceleration |
| ffunc |[derivsFunc](#typedef-derivsFunc)| A function that returns velocity |
|user_data  |```void*```| A pointer to user defined data that will be passed to a derivsFunc when called by the Integrator. |