
Debug Pause is a debugging feature provided by Trick. When turned on, Debug Pause will print the current simulation time and
the name of the current job, and pause the simulation @e before the job is executed. Debug Pause provides a way for the user to step
through a simulation run job by job. The Simulation Control Panel (see @ref SimControlPanel "Sim Control Panel") is the means by which the user does this.
When a simulation is in Freeze mode, clicking the Step button will turn on Debug Pause. Each subsequent Step click will then execute the current job and pause at the next job.
Clicking Freeze will turn Debug Pause off and return to Freeze mode.

All jobs (user jobs and Trick jobs) of the following job class will be paused at:

- initialization
- integration
- scheduled 
- end_of_frame
- shutdown

Debug Pause uses Trick's messaging system to publish each sim time / job name message,
which means it will be sent to any enabled message subscribers (see  [Status Message System](Status-Message-System)).

[Continue to Echo Jobs](Echo-Jobs)
