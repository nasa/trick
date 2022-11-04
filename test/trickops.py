import sys

sys.path.append(sys.argv[1] + "/share/trick/trickops")
# print (sys.path)
from TrickWorkflow import *
class ExampleWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level ):
        # Base Class initialize, this creates internal management structures
        TrickWorkflow.__init__(self, project_top_level=(trick_top_level + "/test/"), log_dir=(trick_top_level +'/test/trickops_logs/'),
            trick_dir=trick_top_level, config_file=(trick_top_level + "/test/test_run.yml"), cpus=3, quiet=quiet)
    def run( self ):

      # Change all the build commands to have the whole path
      full_build_cmd = self.trick_dir + "/bin/trick-CP"
      print (full_build_cmd)
      self.set_build_cmd( full_build_cmd )

      build_jobs      = self.get_jobs(kind='build')
      run_jobs        = self.get_jobs(kind='run')
      analysis_jobs   = self.get_jobs(kind='analyze')
    
      builds_status = self.execute_jobs(build_jobs, max_concurrent=1, header='Executing all sim builds.')
      if builds_status:
        return builds_status

      runs_status   = self.execute_jobs(run_jobs,   max_concurrent=1, header='Executing all sim runs.')
      comparison_result = self.compare()
      analysis_status = self.execute_jobs(analysis_jobs, max_concurrent=1, header='Executing all analysis.')

      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary
      return (builds_status or runs_status or self.config_errors or comparison_result or analysis_status)
    
    def set_build_cmd(self, cmd):
      for i in range(len(self.sims)):
        self.sims[i].build_cmd = cmd

if __name__ == "__main__":
    sys.exit(ExampleWorkflow(quiet=True, trick_top_level=sys.argv[1]).run())
