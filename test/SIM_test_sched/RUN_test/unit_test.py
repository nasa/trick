
from trick.unit_test import *

if trick.mc_get_slave_id() == 0:
    trick_utest.unit_tests.enable();
    trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_sched.xml")

my_integ_loop.getIntegrator(trick.Runge_Kutta_4, 2)
trick.sim_services.exec_set_terminate_time(100.0)

# Keep access to the embedded scheduler without permitting a copy assignment.
scheduler = my_integ_loop.integ_sched
original_verbosity = int(scheduler.verbosity)
scheduler.verbosity = original_verbosity + 1
TRICK_EXPECT_EQ(
    int(my_integ_loop.integ_sched.verbosity),
    original_verbosity + 1,
    "integ_sched_binding",
    "getter_allows_configuration",
)
scheduler.verbosity = original_verbosity

replacement_rejected = False
try:
    my_integ_loop.integ_sched = scheduler
except AttributeError:
    replacement_rejected = True
TRICK_EXPECT_TRUE(
    replacement_rejected, "integ_sched_binding", "replacement_is_read_only"
)
