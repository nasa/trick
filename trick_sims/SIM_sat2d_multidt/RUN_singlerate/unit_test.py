from trick.unit_test import *
trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_sat2d_multidt.xml" )
trick_utest.unit_tests.set_test_name( "Single Rate IntegTest" )

exec(open("RUN_test/common.py").read())

dyn_integloop.set_integ_cycle(0.002)

trick.add_read(5400.0, '''
TRICK_EXPECT_NEAR(dyn.satellite.pos[0], -1324210.5370531, 0.000001 , "Sat2D" , "FinalXPositionCheck" )
TRICK_EXPECT_NEAR(dyn.satellite.pos[1], 6447498.98285128, 0.000001 , "Sat2D" , "FinalYPositionCheck" )
''')
