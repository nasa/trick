trick.frame_log_off()
trick.real_time_disable()
trick.remove_all_external_applications()
trick.exec_set_enable_freeze(False)
trick.exec_set_freeze_command(False)

from trick.unit_test import *
trick_utest.unit_tests.enable()
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_sat2d_multidt.xml" )
trick_utest.unit_tests.set_test_name( "MultiDtIntegTest" )

exec(open("RUN_test/input.py").read())

trick.add_read(5400.0, '''
TRICK_EXPECT_NEAR(dyn.satellite.pos[0], -1324210.53705666, 0.000001 , "Sat2D" , "FinalXPositionCheck" )
TRICK_EXPECT_NEAR(dyn.satellite.pos[1], 6447498.9828508, 0.000001 , "Sat2D" , "FinalYPositionCheck" )
''')
