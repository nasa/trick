
Trick provides a means to gather simulation performance data and view it using Data Products (see [Data Products](Data-Products)).
When the user turns on the Frame Logging feature, Trick will use its Data Recording mechanism to track the following:
- execution time of each Trick and User job, view in Data Products using DP_rt_trickjobs.xml and DP_rt_userjobs.xml
- a timeline showing when each job runs during each job frame, view in Data Products using DP_rt_timeline_init.xml and DP_rt_timeline.xml
- total execution time of all jobs for each frame showing underrruns/overruns, view in Data Products using DP_rt_frame.xml

You can turn frame logging on and off whenever you want to toggle frame logging during execution.  
Caveat:  
You must always turn frame_log_on() first (i.e., in the input file) because it has to create data recording groups at initialization time.
If you don't want to begin frame logging at time 0, then you can simply make your next statement frame_log_off(). After that you can turn it on/off at will.

The following input file example will only frame log from simulation time 500-600 and from 1500 to end of run:

```python
trick.frame_log_on()
trick.frame_log_off()
trick.add_read(500.0, "trick.frame_log_on()")
trick.add_read(600.0, "trick.frame_log_off()")
trick.add_read(1500.0, "trick.frame_log_on()")
```

### User accessible routines

```
int frame_log_on() ;
int frame_log_off() ;
int frame_log_set_max_samples(int num) ;
```

[Continue to Debug Pause](Debug-Pause)
