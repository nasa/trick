# The purpose of this test is to:
#   Execute the first input file to add two variables into MonteCarloMaster.
#   Execute the second input file to add the same variables into 
#     MonteCarloMaster, but removes one of them before monte carlo RUN files
#     are generated.
# Then it compares the variable lists from the monte carlo runs. If they differ,
# the test terminate with a non-zero return.

import os
exename = "S_main_" + os.getenv("TRICK_HOST_CPU") + ".exe"

print("Processing 1st input file for run RUN_remove_variable")
input_file = "RUN_remove_variable/input_a.py"
ret = os.system("./" + exename + " " + input_file)
if ret != 0:
  trick.exec_terminate_with_return(1, "input.py", 16, "Error running " + input_file)

print("Processing 2nd input file for run RUN_remove_variable")
input_file = "RUN_remove_variable/input_b.py"
ret = os.system("./" + exename + " " + input_file)
if ret != 0:
  trick.exec_terminate_with_return(1, "input.py", 22, "Error running " + input_file)

print('Checking if the variable was successfully removed')
ret = os.system("diff -q MONTE_RUN_remove_variable/RUN_0/monte_variables MONTE_RUN_remove_variable/RUN_1/monte_variables > /dev/null")
if ret != 0:
  trick.exec_terminate_with_return(0, "input.py", 27, "variable successfully removed!")
else:
  trick.exec_terminate_with_return(1, "input.py", 29, "variable 'test.x_fixed_value_int' was not removed!")

# To be compatible with our current unit-sim framework, this file has to be a
# simulation input file. Therefore it needs a stop time so it doesn't run forever.
trick.stop(0.0)
