#!/usr/bin/env python3
import os, sys

# Create a valid yml config file describing which sims to consider
yml_content=(
"""
SIM_parse_s_define:
    path: test/SIM_parse_s_define
SIM_python_namespace:
    path: test/SIM_python_namespace
    runs:
        RUN_test/unit_test.py:
SIM_rti:
    path: test/SIM_rti
    runs:
        RUN_test/unit_test.py:
SIM_segments:
    path: test/SIM_segments
    runs:
        RUN_test/input.py:
SIM_stls:
    path: test/SIM_stls
    runs:
        RUN_test/input.py:
        RUN_test/unit_test.py:
SIM_swig_template_scoping:
    path: test/SIM_swig_template_scoping
SIM_target_specific_variables:
    path: test/SIM_target_specific_variables
SIM_test_abstract:
    path: test/SIM_test_abstract
    runs:
        RUN_test/input.py:
SIM_test_dp:
    path: test/SIM_test_dp
    runs:
        RUN_test/unit_test.py:
        RUN_test/input.py:
SIM_test_dr:
    path: test/SIM_test_dr
    runs:
        RUN_test/unit_test.py:
SIM_test_inherit:
    path: test/SIM_test_inherit
    runs:
        RUN_test/input.py:
SIM_test_io:
    path: test/SIM_test_io
    runs:
        RUN_test/unit_test.py:
SIM_test_ip:
    path: test/SIM_test_ip
    runs:
        RUN_test/unit_test.py:
SIM_test_sched:
    path: test/SIM_test_sched
    runs:
        RUN_test/input.py:
        RUN_test/unit_test.py:
SIM_test_templates:
    path: test/SIM_test_templates
    runs:
        RUN_test/unit_test.py:
SIM_threads:
    path: test/SIM_threads
    runs:
        RUN_test/sched.py:
        RUN_test/amf.py:
        RUN_test/async.py:
        RUN_test/unit_test.py:
SIM_threads_simple:
    path: test/SIM_threads_simple
    runs:
        RUN_test/input.py:
        RUN_test/sched.py:
        RUN_test/async.py:
SIM_trickcomm:
    path: test/SIM_trickcomm
    runs:
        RUN_test/input.py:
SIM_ball_L2:
    path: trick_sims/Ball/SIM_ball_L2
SIM_ball_L3:
    path: trick_sims/Ball/SIM_ball_L3
SIM_amoeba:
    path: trick_sims/Cannon/SIM_amoeba
SIM_cannon_aero:
    path: trick_sims/Cannon/SIM_cannon_aero
SIM_cannon_analytic:
    path: trick_sims/Cannon/SIM_cannon_analytic
SIM_cannon_eulercromer:
    path: trick_sims/Cannon/SIM_cannon_eulercromer
SIM_cannon_jet:
    path: trick_sims/Cannon/SIM_cannon_jet
SIM_cannon_numeric:
    path: trick_sims/Cannon/SIM_cannon_numeric
SIM_monte:
    path: trick_sims/Cannon/SIM_monte
SIM_Ball++_L1:
    path: trick_sims/SIM_Ball++_L1/
SIM_contact:
    path: trick_sims/SIM_contact
SIM_lander:
    path: trick_sims/SIM_lander
SIM_msd:
    path: trick_sims/SIM_msd
SIM_parachute:
    path: trick_sims/SIM_parachute
SIM_rocket:
    path: trick_sims/SIM_rocket
SIM_sat2d:
    path: trick_sims/SIM_sat2d
SIM_satellite:
    path: trick_sims/SIM_satellite
SIM_sun:
    path: trick_sims/SIM_sun
SIM_wheelbot:
    path: trick_sims/SIM_wheelbot
""")
f = open("/tmp/config.yml", "w")
f.write(yml_content)
f.close()

from TrickWorkflow import *
class ExampleWorkflow(TrickWorkflow):
    def __init__( self, quiet, trick_top_level='/tmp/trick'):
        # Real projects already have trick somewhere, but for this test, just clone it
        if not os.path.exists(trick_top_level):
          os.system('cd %s && git clone https://github.com/nasa/trick' % (os.path.dirname(trick_top_level)))
        # Base Class initialize, this creates internal management structures
        TrickWorkflow.__init__(self, project_top_level=trick_top_level, log_dir='/tmp/',
            trick_dir=trick_top_level, config_file="/tmp/config.yml", cpus=3, quiet=quiet)
    def run( self):
      build_jobs      = self.get_jobs(kind='build')
      run_jobs        = self.get_jobs(kind='run')

      builds_status = self.execute_jobs(build_jobs, max_concurrent=3, header='Executing all sim builds.')
      runs_status   = self.execute_jobs(run_jobs,   max_concurrent=3, header='Executing all sim runs.')
      self.report()           # Print Verbose report
      self.status_summary()   # Print a Succinct summary
      return (builds_status or runs_status or self.config_errors)
if __name__ == "__main__":
    sys.exit(ExampleWorkflow(quiet=(True if len(sys.argv) > 1 and 'quiet' in sys.argv[1] else False)).run())
