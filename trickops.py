import sys
import os

sys.path.append(sys.argv[1] + "/share/trick/trickops")

from TrickWorkflow import *
from WorkflowCommon import Job

max_retries = 5

class SimTestWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level ):
        # Create the trick_test directory if it doesn't already exist
        if not os.path.exists(trick_top_level + "/trick_test"):
          os.makedirs(trick_top_level + "/trick_test")

        # Base Class initialize, this creates internal management structures
        num_cpus = os.cpu_count() if os.cpu_count() is not None else 8
        TrickWorkflow.__init__(self, project_top_level=(trick_top_level), log_dir=(trick_top_level +'/trickops_logs/'),
            trick_dir=trick_top_level, config_file=(trick_top_level + "/test_sims.yml"), cpus=num_cpus, quiet=quiet)
         
    def run( self ):

      build_jobs      = self.get_jobs(kind='build')
      run_jobs        = self.get_jobs(kind='run')
      analysis_jobs   = self.get_jobs(kind='analyze')

      # This job in SIM_stls dumps a checkpoint that is then read in and checked by RUN_test/unit_test.py in the same sim
      # This is a workaround to ensure that this run goes first.
      first_phase_jobs = []
      stl_dump_job = self.get_sim('SIM_stls').get_run(input='RUN_test/setup.py').get_run_job()
      first_phase_jobs.append(stl_dump_job)
      run_jobs.remove(stl_dump_job)

      # Same with SIM_checkpoint_data_recording - half the runs dump checkpoints, the others read and verify.
      # Make sure that the dump checkpoint runs go first.
      num_dump_runs = int(len(self.get_sim('SIM_checkpoint_data_recording').get_runs())/2)
      for i in range(num_dump_runs):
        job = self.get_sim('SIM_checkpoint_data_recording').get_run(input=('RUN_test' + str(i+1) + '/dump.py')).get_run_job()
        first_phase_jobs.append(job)
        run_jobs.remove(job)

      # Some tests fail intermittently for reasons not related to the tests themselves, mostly network weirdness.
      # Allow retries so that we can still cover some network-adjacent code
      retry_allowed_sims = self.get_sims(labels='retries_allowed')
      retry_allowed_jobs = [run.get_run_job() for run in [item for sublist in [sim.get_runs() for sim in retry_allowed_sims] for item in sublist]]
      for job in retry_allowed_jobs:
         run_jobs.remove(job)


      builds_status = self.execute_jobs(build_jobs, max_concurrent=self.cpus, header='Executing all sim builds.')
      first_phase_run_status = self.execute_jobs(first_phase_jobs, max_concurrent=self.cpus, header="Executing required first phase runs.")
      runs_status   = self.execute_jobs(run_jobs,   max_concurrent=self.cpus, header='Executing all sim runs.')

      # Run the retry_allowed jobs
      self.execute_jobs(retry_allowed_jobs, max_concurrent=self.cpus, header='Executing retry-allowed runs.')
      
      # If anything failed, try it again up to max_retries times
      all_retried_status = 0
      final_retry_jobs = []
      for sim in retry_allowed_sims:
        failing_runs = [run for run in sim.get_runs() if run.get_run_job().get_status() == Job.Status.FAILED]
        for run in failing_runs:
          status, final_job = self.retry_job(sim, run, max_retries)
          final_retry_jobs += [final_job]
          all_retried_status = all_retried_status or status

      comparison_result = self.compare()
      analysis_status = self.execute_jobs(analysis_jobs, max_concurrent=self.cpus, header='Executing all analysis.')

      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary

      # Dump failing logs
      jobs = build_jobs + first_phase_jobs + run_jobs + final_retry_jobs
      for job in jobs:
        if job.get_status() == Job.Status.FAILED:
          print("Failing job: ", job.name)
          print ("*"*120)
          print(open(job.log_file, "r").read())
          print ("*"*120, "\n")

      return (builds_status or runs_status or first_phase_run_status or all_retried_status or self.config_errors or comparison_result or analysis_status)

    # Retries a job up to max_retries times and adds runs to the sim
    # Returns tuple of (job_status, final retry job)
    def retry_job(self, sim, run, max_retries):
      tries = 0
      job_failing = 1
      retry_run = None
      retry_job = None
      while tries < max_retries and job_failing:
        tries += 1
        retry_run = TrickWorkflow.Run(sim_dir=run.sim_dir, input=run.input, binary=run.binary, returns=run.returns,log_dir=run.log_dir)
        retry_job = retry_run.get_run_job()
        retry_job.name = retry_job.name + "_retry_" + str(tries)
        job_failing = self.execute_jobs([retry_job], max_concurrent=1, header="Retrying failed job")
        sim.add_run(retry_run)
    
      return (job_failing, retry_job)


if __name__ == "__main__":
    should_be_quiet = os.getenv('CI') is not None
    sys.exit(SimTestWorkflow(quiet=should_be_quiet, trick_top_level=sys.argv[1]).run())
