
Echo Jobs is a handy debugging toggle provided by Trick. When turned on, Echo Jobs will print the current simulation time and
the name of the current job being executed. Echo Jobs is a convenient way to see the order of job execution and narrow down when
and where something occurs during a simulation run.

All jobs (user jobs and Trick jobs) of the following job class will be echoed:

- initialization
- integration
- scheduled 
- end_of_frame
- shutdown

Echo Jobs uses Trick's messaging system to publish each sim time / job name message,
which means it will be sent to any enabled message subscribers (see  @ref StatusMessageSystem "Status Message System").

### User accessible routines

```
int echo_jobs_on() ;
int echo_jobs_off() ;
```

[Continue to Variable Server](Variable-Server)
