exec(open("RUN_test/input.py").read())

from trick.unit_test import *

trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_multidt.xml" )
trick_utest.unit_tests.set_test_name( "MultiDt" )

######################################################################################################################

test_suite = "multi_dt_integ"


trick.add_read(300.0,
'''
TRICK_EXPECT_NEAR( ball.output_position[0], -7.627189 , 0.000001 , test_suite , "FinalXPositionCheck" )
TRICK_EXPECT_NEAR( ball.output_position[1],  3.131818 , 0.000001 , test_suite , "FinalYPositionCheck" )
'''
)
