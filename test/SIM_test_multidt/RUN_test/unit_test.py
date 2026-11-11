exec(open("RUN_test/input.py").read())

from trick.unit_test import *

trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_multidt.xml" )
trick_utest.unit_tests.set_test_name( "MultiDt" )

######################################################################################################################

test_suite = "multi_dt_integ"


# The scheduler is embedded in its sim object. Python may configure it through
# the getter, but must not replace it by copying another scheduler.
scheduler = my_integ_loop.integ_sched
original_verbosity = int(scheduler.verbosity)
scheduler.verbosity = original_verbosity + 1
TRICK_EXPECT_EQ(
    int(my_integ_loop.integ_sched.verbosity),
    original_verbosity + 1,
    test_suite,
    "scheduler_getter_allows_configuration",
)
scheduler.verbosity = original_verbosity

replacement_rejected = False
try:
    my_integ_loop.integ_sched = scheduler
except AttributeError:
    replacement_rejected = True
TRICK_EXPECT_TRUE(
    replacement_rejected, test_suite, "scheduler_replacement_is_read_only"
)


trick.add_read(300.0,
'''
TRICK_EXPECT_NEAR( ball.output_position[0], -7.663642 , 0.000001 , test_suite , "FinalXPositionCheck" )
TRICK_EXPECT_NEAR( ball.output_position[1],  3.145546 , 0.000001 , test_suite , "FinalYPositionCheck" )
'''
)
