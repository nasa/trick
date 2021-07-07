
Trick provides a state integration capability described by the inputs below.
To use these options a developer must develop application code which interfaces the application states with
the Trick integration services.
The integration job class is designed to accommodate the application state to Trick integration service interface.

All integration class jobs must return an integer value which represents the current integration pass identifier.
If all integration passes are complete, the job must return a zero.

The code below represents a simple integration job implementation.

```
/*********** TRICK HEADER **************
PURPOSE:   (State Integration Job)
...
CLASS:     (integration)
...
*/
#include "ip_state.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"

int integration_test( IP_STATE* s)
{
    int ipass;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &s->pos[0],
        &s->pos[1],
        &s->vel[0],
        &s->vel[1],
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &s->vel[0],
        &s->vel[1],
        &s->acc[0],
        &s->acc[1],
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &s->pos[0],
        &s->pos[1],
        &s->vel[0],
        &s->vel[1],
        NULL
    );

    /* RETURN */
    return(ipass);
}
```

The <i> integrate() </i> function, declared externally, is the function which physically integrates the states.
This function uses the input parameters defined in Table 18 and 19 to integrate any set of states and derivatives.

First, the states must be loaded,<i> load_state() </i>.
Notice in the example code that both position and velocity are loaded into the state array.
This is because the integrators are primarily 1st order differential equation integrators, which means that
velocities are integrated to positions independently from the accelerations being integrated to velocities.
Hence, the velocity is a state and the acceleration is its derivative,
just as the position is a state and velocity is its derivative.
From the 2 degree of freedom code example, there are four states: two position and two velocity.

Next, the derivative of the position (velocity) and the derivative of the velocity (acceleration) must be loaded,
<i> load_deriv() </i>.  The integration job class is designed to be called once for each intermediate
pass of a multi-pass integrator.  For example the Runge_Kutta_4 integrator will make 4 separate derivative
evaluations and stores the resulting state from each intermediate pass separately so that they may be
combined and weighted to create a "true" state for the specified time step.  The intermediate_step parameter
defines the current intermediate step ID for the integrator.  This parameter is initialized to zero by the
executive and managed by the <i> integrate() </i> function.

With the states and derivatives loaded into the appropriate integrator arrays, the <i> integrate() </i> function
must be called to integrate the states through a single intermediate step of the selected integration scheme.
The integrated states must then be unloaded, <i> unload_state() </i>.

If a developer wishes to use their own integration scheme, then the <i> integrate() </i> function source code
should be reviewed so that the proper interfaces can be maintained.  The <i> integrate() </i> source code is
located in the ${TRICK_HOME}/trick_source/sim_services/integ/integrate.c file.

### Integrator Control Inputs

There can be any number of <i> integration </i> class jobs listed within the S_define file;
each integration job should have an associated <i> IntegLoop </i> declaration.
The available inputs for state integration control are listed in Table 18.

Table 18 State Integration Control Inputs
<table>
 <tr>
  <th width=375>Name</th>
  <th>Default</th>
  <th>Description</th>
 </tr>
 <tr>
  <td>getIntegrator(Integrator_type, unsigned int, double)</td>
  <td>No default value</td>
  <td>Tell Trick the Integrator scheme and the number of state variables.
      A call to this function is required otherwise a runtime error is generated.</td>
 </tr>
 <tr>
  <td>set_first_step_deriv(bool)</td>
  <td>True</td>
  <td>True=perform derivative evaluation for the first pass of the integrator;
      False=use the derivative evaluation from the last pass of the previous integration cycle.</td>
 </tr>
 <tr>
  <td>set_last_step_deriv(bool)</td>
  <td>False</td>
  <td>True=perform derivative evaluation for the last pass of the integrator;
      False=do not perform derivative evaluation for the last pass of the integrator.</td>
 </tr>
</table>

- <b> getIntegrator(Alg, State_size, Dt) </b>:  The <b> Alg </b> parameter is an enumerated type which currently
   has nine possible values.  These values and information about the associated integrator is shown in Table 19.
   The <b> State_size </b> parameter is the number of states that are to be integrated. This includes position
   <i> and </i> velocity states; e.g. for a three axis translational simulation, there would be three position
   states and three velocity states, hence the second parameter would equal 6 states.
   The <b> Dt </b> parameter is the integration frequency; however, this parameter is ignored unless using the
   <i> Integration </i> class stand-alone.  The frequency is defined in the S_define when using integration within Trick.
- <b> set_first_step_deriv(first_step) </b>: The <b> first_step </b> parameter is a boolean.  If <b> True </b> then
   Trick will run the derivative jobs for the first integration step.  If <b> False </b> then Trick will run only
   the integration jobs for the first integration step.
- <b> set_last_step_deriv(last_step) </b>: The <b> last_step </b> parameter is a boolean.  If <b> True </b> then
   Trick will run the derivative jobs after the last integration step.  If <b> False </b> then Trick will not run
   the derivative jobs after the last integration step.

Table 19 State Integration Options
<table>
 <tr>
  <th>Option</th>
  <th>Accuracy</th>
  <th>DiffEQ</th>
  <th># Deriv</th>
  <th>Comments</th>
 </tr>
 <tr>
  <td>Euler</td>
  <td>1st Order</td>
  <td>1st Order</td>
  <td>1</td>
  <td>yn + 1 = yn + y'n*dt</td>
 </tr>
 <tr>
  <td>Euler_Cromer</td>
  <td>2nd Order</td>
  <td>2nd Order</td>
  <td>2</td>
  <td>yn + 1 = yn + y'n + 1*dt</td>
 </tr>
 <tr>
  <td>ABM_Method</td>
  <td></td>
  <td>  </td>
  <td>  </td>
  <td>Adams-Bashforth-Moulton Predictor Corrector</td>
 </tr>
 <tr>
  <td>Nystrom_Lear_2</td>
  <td>2nd Order</td>
  <td>2nd Order </td>
  <td>1</td>
  <td>4th order accuracy for orbital state propagation, circular motion</td>
 </tr>
 <tr>
  <td>Runge_Kutta_2</td>
  <td>2nd Order</td>
  <td>2nd Order </td>
  <td>2</td>
  <td>Good general purpose integrator</td>
 </tr>
 <tr>
  <td>Modified_Midpoint_4</td>
  <td>4th Order</td>
  <td>2nd Order </td>
  <td>3</td>
  <td>Good accuracy with less derivative evaluations, be careful with high frequency statesr</td>
 </tr>
 <tr>
  <td>Runge_Kutta_4</td>
  <td>4th Order</td>
  <td>1st Order </td>
  <td>4</td>
  <td>Good general purpose integrator, although a little time consuming</td>
 </tr>
 <tr>
  <td>Runge_Kutta_Gill_4</td>
  <td>4th Order</td>
  <td>1st Order </td>
  <td>4</td>
  <td>Good general purpose integrator, although a little time consuming</td>
 </tr>
 <tr>
  <td>Runge_Kutta_Fehlberg_45</td>
  <td>5th Order</td>
  <td>1st Order </td>
  <td>6</td>
  <td>Designed for larger time steps and smooth states, orbital state propagator</td>
 </tr>
 <tr>
  <td>Runge_Kutta_Fehlberg_78</td>
  <td>8th Order</td>
  <td>1st Order </td>
  <td>12</td>
  <td>Designed for larger time steps and smooth states, orbital state propagator</td>
 </tr>
 <tr>
  <td>User_Defined</td>
  <td>N/A</td>
  <td>N/A</td>
  <td>N/A</td>
  <td>Used to bypass trick integration utilities</td>
 </tr>
</table>

The <b> Option </b> column are the integration algorithm options.
The <b> Accuracy </b> column gives the order of accuracy for the integrator.
The <b> DiffEQ </b> column gives the order of teh differential equation set the integrator formulation assumes.
For example, a 1st order DiffEQ integrator integrates accelerations to velocities independently of the velocity
to position integration.  However, a 2nd order DiffEQ integrator integrates the velocity to position states
dependent on the acceleration to velocity state integration.  The # <b> Deriv </b> column specifies the number
of derivative evaluations performed to integrate across a full time step (also known as the number of
integration passes).  The <b> Comments </b> column gives some special notes for the usage of each integrator.

[Continue to Frame Logging](Frame-Logging)
