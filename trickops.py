import sys
import os

sys.path.append(sys.argv[1] + "/share/trick/trickops")
# print (sys.path)
from TrickWorkflow import *
class SimTestWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level ):
        # Create the trick_test directory if it doesn't already exist
        if not os.path.exists(trick_top_level + "/trick_test"):
          os.makedirs(trick_top_level + "/trick_test")

        # Base Class initialize, this creates internal management structures
        TrickWorkflow.__init__(self, project_top_level=(trick_top_level), log_dir=(trick_top_level +'/trickops_logs/'),
            trick_dir=trick_top_level, config_file=(trick_top_level + "/test_sims.yml"), cpus=3, quiet=True)
    def run( self ):

      build_jobs      = self.get_jobs(kind='build')
      run_jobs        = self.get_jobs(kind='run')
      analysis_jobs   = self.get_jobs(kind='analyze')

      # This job dumps a checkpoint that is then read in and checked by RUN_test/unit_test.py in the same sim
      # This is a workaround to ensure that this run goes first.
      first_phase_job = self.get_sim('SIM_stls').get_run(input='RUN_test/setup.py').get_run_job()
      run_jobs.remove(first_phase_job)

      builds_status = self.execute_jobs(build_jobs, max_concurrent=8, header='Executing all sim builds.')
      first_phase_run_status = self.execute_jobs([first_phase_job], max_concurrent=8, header="Executing required first phase runs.")
      runs_status   = self.execute_jobs(run_jobs,   max_concurrent=8, header='Executing all sim runs.')
      comparison_result = self.compare()
      analysis_status = self.execute_jobs(analysis_jobs, max_concurrent=8, header='Executing all analysis.')

      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary
      return (builds_status or runs_status or first_phase_run_status or self.config_errors or comparison_result or analysis_status)

if __name__ == "__main__":
    sys.exit(SimTestWorkflow(quiet=True, trick_top_level=sys.argv[1]).run())
