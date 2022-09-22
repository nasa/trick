# Instruct sim to generate MC files for RUN_verif.
# This could be done in a top-level MC-launch script
test.mc_master.activate("RUN_nominal")
test.mc_master.set_num_runs(2)
test.mc_master.generate_meta_data = True
test.mc_master.input_file_name = "input_a.py"
test.mc_master.minimum_padding = 3



# Standard if-tests for a regular multi-purpose input file, allowing for a
# MC-implementation of a general scenario.

# NOTE: in this case, the first test is redundant because this input file is
#       ALWAYS going to have mc-master be active.  But this is likely to get
#       copied and used as a template.

# Quick breakdown:
#  - if running with MC:
#    (this test allows a general input file to have MC-specific content)
#
#     - setup logging and any other MC-specific configurations
#
#     - if generating dispersions, generate them.
#       (This test separates out the execution of pass#1 (which generates the
#        dispersions) from that of pass#2 (which executes with those
#        dispersions). Without this test blocking the generation on pass#2, the
#        dispersions would get regenerated for every actual run, which is
#        completely unnecessary.)
if test.mc_master.active:
  # Logging
  exec(open("Log_data/log_nominal.py").read())

  if test.mc_master.generate_dispersions:
    exec(open("Modified_data/monte_variables.py").read())

trick.stop(1)
