
from trick.unit_test import *

if trick.mc_get_slave_id() == 0:
    trick_utest.unit_tests.enable();
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_sched.xml")

my_integ_loop.getIntegrator(trick.Runge_Kutta_4, 2)
trick.sim_services.exec_set_terminate_time(100.0)
