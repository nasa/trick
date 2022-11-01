import sys

sys.path.append(sys.argv[1] + "/share/trick/trickops")
# print (sys.path)
from TrickWorkflow import *
class ExampleWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level ):
        # Base Class initialize, this creates internal management structures
        TrickWorkflow.__init__(self, project_top_level=(trick_top_level + "/test/"), log_dir=(trick_top_level +'/test/trickops_logs/'),
            trick_dir=trick_top_level, config_file=(trick_top_level + "/test/test_run.yml"), cpus=3, quiet=quiet)
    def run( self):
      build_jobs      = self.get_jobs(kind='build')
      run_jobs        = self.get_jobs(kind='run')
      analysis_jobs   = self.get_jobs(kind='analyze')

      builds_status = self.execute_jobs(build_jobs, max_concurrent=3, header='Executing all sim builds.')
      runs_status   = self.execute_jobs(run_jobs,   max_concurrent=3, header='Executing all sim runs.')
      comparison_result = self.compare()
      analysis_status = self.execute_jobs(analysis_jobs, max_concurrent=3, header='Executing all analysis.')

      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary
      return (builds_status or runs_status or self.config_errors or comparison_result or analysis_status)

if __name__ == "__main__":
    sys.exit(ExampleWorkflow(quiet=True, trick_top_level=sys.argv[1]).run())