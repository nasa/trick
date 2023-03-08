#!/usr/bin/env python3
import os, sys

sys.path.append(os.path.abspath("../../../share/trick/trickops/"))

from TrickWorkflow import *
class ExampleWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level=os.path.abspath("../../../")):
      # Base Class initialize, this creates internal management structures
      if not(os.path.isdir(os.path.abspath("./MCGTrickOpsLog"))):
        logDirectory = "MCGTrickOpsLog"
        logDirectoryParent = os.path.abspath(".")
        logPath = os.path.join(logDirectoryParent, logDirectory)
        os.mkdir(logPath)
      TrickWorkflow.__init__(self, project_top_level=trick_top_level,
        log_dir=os.path.join(trick_top_level,'test/SIM_mc_generation/MCGTrickOps/MCGTrickOpsLog'),
        trick_dir=trick_top_level, 
        config_file="test/SIM_mc_generation/MCGTrickOps/MCGenerationTest.yml",
        cpus=3, quiet=quiet)
    def run( self):
      build_jobs      = self.get_jobs(kind='build')
      run_jobs        = self.get_jobs(kind='run')
      builds_status = self.execute_jobs(build_jobs, max_concurrent=3, header='Executing all sim builds.')
      runs_status   = self.execute_jobs(run_jobs,   max_concurrent=1, header='Executing all sim runs.')
      c = self.compare()
      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary
      return (builds_status or runs_status or self.config_errors or c)
if __name__ == "__main__":
      ExampleWorkflow(quiet=True).run()
