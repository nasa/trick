# The Monte Carlo tool uses a double execution of the S-main:
# - pass #1 uses the scenario input.py file to process the variables identified
#   for dispersion.  A specified number, N,  of values {v_1, ..., v_N} is
#   generated for each variable v, with the values constrained by the specified
#   distribution of v; N is specified in the input file.
#   A set of N files, {RUN_1/monte_input.py, ... , RUN_N/monte_input.py} is
#   created, with each file containing one of the set of values for each
#   variable. Once these files are generated, the simulation is complete for
#   pass #1 and it terminates.
# - pass #2 uses one of the generated files (monte_input.py) as the input file
#   for a regular execution of the simulation.  There will typically be many
#   executions of the sim, one for each of the generated monte_input.py files.

# This input file provides one example of how to test this two-pass process,
# although it is admittedly a bit convoluted and hard to read.  TODO: Once
# TrickOps is capable of operating with this monte-carlo implementation, that
# framework can manage both the generation and local execution of generated
# monte_input.py files, removing the need for this type of "sim that launches a
# sim" test methodology -Jordan 10/2022

# For the purpose of expedient testing, we generate and run only 2 files.
# This is sufficient to demonstrate "multiple" without unnecessarily
# burning CPU time.

import os
exename = "S_main_" + os.getenv("TRICK_HOST_CPU") + ".exe"

# Pass #1 Generate the set of scenarios with unique dispersions
print("Processing Pass #1 for run RUN_nominal")
input_file = "RUN_nominal/input_a.py"
ret = os.system("./" + exename + " " + input_file)
if ret != 0:
  trick.exec_terminate_with_return(1, "double_pass.py", 34, "Error running " + input_file)

# Pass #2 Run the scenarios. Logged data will go into each scenario's folder
print("")
print("")
print("Processing Pass #2 for run RUN_nominal")
for ii in range(2):
  input_file = "MONTE_RUN_nominal"+"/RUN_00%d/monte_input_a.py" %ii
  print ("****************  %s" %input_file)
  ret = os.system("./" + exename + " " + input_file)
  if ret != 0:
    trick.exec_terminate_with_return(1, "double_pass.py", 43, "Error running " + input_file)

# To be compatible with our current unit-sim framework, this file has to be a
# simulation input file. Therefore it needs a stop time so it doesn't run forever.
trick.stop(0.0)
