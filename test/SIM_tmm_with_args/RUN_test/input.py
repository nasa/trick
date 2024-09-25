import math
from trick.unit_test import *

trick.sim_control_panel_set_enabled(True)
trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)

#Use tmm_alloc_args from input file - thanks convert_swig!
alloc_test.atwargs_input_file = trick.AllocTestWithArguments.alloc(5, 7.0)





