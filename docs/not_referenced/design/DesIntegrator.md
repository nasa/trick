
# Integrators

Integration is a numerical process of accumulating change.  It is commonly used to propagate simulation states from one
time step to the next.

Trick's integration scheme helps a simulation developer to perform integration, using one of many well-known integration
algorithms or to define his own.

To do this, Trick defines two job classes specifically for integration:
1) Derivative Class Job
2) Integration Class Job

At initialization time, the job scheduler runs all derivative jobs once.  During run-time, Derivative and Integration class
jobs are then alternately executed one or more times at the top of the rate group frame.  The integration algorithm chosen
by the user determines the number of times that they are executed per frame.  For example, for Runge-Kutta 2 they will
alternately be called 2 times.

## Derivative Class Job

The user implements derivative class job functions which are expected to calculate derivatives (to be integrated).  Derivative job functions may pass any number of arguments.  They may also be written to return any value.  Their return value is irrelevant to the scheduler.  The unique feature of a derivative class job is how it's scheduled.  It is always called right before integration class job(s) in the same sim_object.

## Integrator Class Job

The user also implements integration class jobs functions which are expected to integrate the derivatives calculated by the derivative class jobs in the same sim object. Like derivative job functions, integration job functions may take any number of arguments. But, the return value of an integration function is significant.

A non-zero return value tells the job scheduler ( see exec_scheduled_modules(), in S_source.c) that integration is not yet complete, and that the derivative and integration class job functions should again be called for the current (t) frame.  A zero return value means that integration is complete for the current frame.

## Integrator integrate() Function Interface

To aid in the implementation of an integration job function, Trick provides the integrate() function:

```
int integrate() ;
```

The Integrator object stores the input (derivative) values to be integrated, intermediate values, and the resultant
(integrated) values. 

To use <I>integrate()</I>, the user is expected to:

1. Declare an IntegLoop in the S_define with the frequency the integration/derivative jobs.
1. Call getIntegrator(< Integration Algorithm >, <# of variables to integrate>) in the input file for each IntegLoop.
1. In the integration class job:<BR>
    1.  Populate the Integrator object with the state-values:
       `load_state( <state variable>, ..., NULL);`
    1. and state-derivative values:
       `load_deriv( <state deriv variable>, ..., NULL);`
    1. call the Trick <I>integrate()</I> function:
       `<int> = integrate();`
    1. unload the results from:
       `unload_state(<state variable, ..., NULL);`
    1. and return the integer value returned from the <I>integrate()</I> function.

Each time <I>integrate()</I> is called it will make progress in its estimate of the new state.

That progress is recorded in the element Integrator->state_ws.  After calling <I>integrate()</I>, a non-zero value
in Integrator->intermediate_step, indicates that the integration is not yet complete; that <I>integrate()</I> needs
to be called again.  A zero value in Integartor->intermediate_step means that integration is done.  The integration algorithms available are listed in the table below.

<table>
<tr><th>Integration Algorithm</th></tr>
<tr><td>Euler</td></tr>
<tr><td>Euler_Cromer</td></tr>
<tr><td>Nystrom_Lear_2</td></tr>
<tr><td>Runge_Kutta_2</td></tr>
<tr><td>Modified_Midpoint_4</td></tr>
<tr><td>Runge_Kutta_4</td></tr>
<tr><td>Runge_Kutta_Gill_4</td></tr>
<tr><td>Runge_Kutta_Fehlberg_45</td></tr>
<tr><td>Runge_Kutta_Fehlberg_78</td></tr>
<tr><td>ABM_Method</td></tr>
</table>

## Derivative and Integration Class Job Scheduling

Derivative and Integration class jobs are scheduled/called by the IntegLoop Scheduler to which they are assigned.

A new IntegLoopScheduler class is created for each IntegLoop defined in the S_define.  The sim_objects that are
assigned to the IntegLoopScheduler are added to the scheduler with a call to <I>add_sim_object()</I>.  Each 
IntegLoopScheduler is registered with the Executive when the default_data class jobs are run.  Also, <I>rebuild_jobs()</I>
is called to add jobs to the proper queue for each sim_object assigned to the IntegLoopScheduler during default_data 
class jobs. When the initialization class jobs are run, all of the derivative class jobs will be run at time equals 0.
Based on the user-defined fequency, the Executive calls the appropriate IntegLoopScheduler <I>integrate()</I> function.  

### IntegLoopScheduler Integrate() Function Design

The following happens for each IntegLoopScheduler <I>integrate()</I> function call:
- All pre-integration class jobs are called.
- If the variable first_step_deriv is TRUE, then all derivative class jobs are called.
- Call all integration class jobs, each integration class job will be called repeatedly until the specific integration
  algorithm returns a zero value.
- Call all derivative class jobs.
- Call the dynamic_event class jobs.

## Integration Algorithms

The specific Integrators that Trick supports are listed in the above table.  Each of these integration algorithms
is well known in the world of numerical methods and therefore will not be described.  The implementation of the 
algorithms are included in Trick release packages, but are not maintained by Trick.  The ER7 division maintains
an er7_utils configuration that contains the integrator code that is used by Trick.
@see Trick::Integrator
