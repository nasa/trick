# ScheduledJobQueue Design

The Scheduler typically calls upon the ScheduledJobQueue to add
a job, find the next job that matches the current sim time, get the next
job call time, and instrument the job queue.

Listed below are the design for each use case of the ScheduledJobQueue.

## Adding a job

Adding a job to the queue increases the queue size by 1.  Jobs are ordered by
job class, then phase, then sim object id, and finally job_id.  The job_id increases
with the ordering of jobs in a sim object.

@copydetails Trick::ScheduledJobQueue::push()
Trick::ScheduledJobQueue::push()

## Adding a job ignoring sim object order.

Somethimes To add a job to the queue, Setting the sim object id to a large nnumber effectively
removes the sim object id in the sorting.

@copydetails Trick::ScheduledJobQueue::push_ignore_sim_object()
Trick::ScheduledJobQueue::push_ignore_sim_object()

## Find Next Job with Current Sim Time

Advances curr_index to next job in list who's next call time matches the incoming time
argument.  If the job is not a system_job as determined by the scheduler, the next job
call time is calculated before returning. (system_jobs set their own next job call time).
Also track the next_job_time which will be used to advance simulation time 
at the end of frame.

@copydetails Trick::ScheduledJobQueue::find_next_job()
Trick::ScheduledJobQueue::find_next_job()

## Get the Next Job

A simple get the next job in the list.

@copydetails Trick::ScheduledJobQueue::get_next_job()
Trick::ScheduledJobQueue::get_next_job()

## Get the next job call time

As the find_next_job routine traverses the queue of jobs searching for jobs that match the
current simulation time step, it also saves the time of the next lowest job
call time.  This is the next time step that the scheduler will use when
advancing simulation time.

@copydetails Trick::ScheduledJobQueue::get_next_job_call_time()
Trick::ScheduledJobQueue::get_next_job_call_time()

## Test Next Job Call Time

Some job types reschedule themselves and the next job call time are not tested by the find next job routine.
The test next job call time allows these job to check their next call time against the overall next
job call time.  If the current job next job call time is lower than the overall job call time, the
overall next job call time is set to the current job's next call time.

@copydetails Trick::ScheduledJobQueue::test_next_job_call_time()
Trick::ScheduledJobQueue::test_next_job_call_time()

## Instrument a job before all jobs in the queue

To add an instrumentation job before a job or all jobs in the queue.  Requirement [@ref r_exec_instrument_0]

@copydetails Trick::ScheduledJobQueue::instrument_before(JobData * , string , InstrumentationEvent * )
Trick::ScheduledJobQueue::instrument_before(JobData * , string , InstrumentationEvent * )

## Instrument a job after all jobs in the queue

To add an instrumentation job after a job or all jobs in the queue.  Requirement [@ref r_exec_instrument_2]

@copydetails Trick::ScheduledJobQueue::instrument_after(JobData * , string , InstrumentationEvent * )
Trick::ScheduledJobQueue::instrument_after(JobData * , string , InstrumentationEvent * )

## Remove an instrumentation job

To remove an instrumentation job from the queue.  Requirement [@ref r_exec_instrument_4]

@copydetails Trick::ScheduledJobQueue::instrument_remove(string , InstrumentationEvent * )
Trick::ScheduledJobQueue::instrument_remove(string , InstrumentationEvent * )

