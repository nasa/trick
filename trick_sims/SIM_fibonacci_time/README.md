# ```SIM_fibonacci_time```

This simulation is simply a demonstration of "automatic" job scheduling.

In this particular simulation, we run a "scheduled" job every second, 
and an "automatic" job when the elapsed time is equal to a Fibonacci number of seconds.

"automatic" jobs run once, at t=0. There after, they must reschedule themselves.

This is accomplished using the following two functions:

```
Trick::JobData * exec_get_curr_job();
```
```
JobData::int set_next_call_time(long long time_tics);
```
The first function retrieves the ```JobData``` of the job that is currently being executed, i.e., our "automatic" job. The second function allows us to set the time  when the job will be called again. Notice that we have to specify the simulation time in time-tics.

To get the current simulation time in time-tics we use:

```
long long exec_get_time_tics( void );
```

And, to get the number of time tics per second, so we can convert between simulation time in seconds and time-tics, we use the following awkwardly name function:

```
int exec_get_time_tic_value( void );
```
