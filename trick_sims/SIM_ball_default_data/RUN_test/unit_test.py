# $Id: unit_test.py 1185 2010-10-25 22:09:41Z wwoodard $

execfile("RUN_test/input.py")

# Disable real-time (if applicable)
trick.real_time_disable()
# Disable all GUIs
trick_sys.sched.set_freeze_command(False)
trick.remove_all_external_applications()

trick_utest.unit_tests.enable() ;
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_ball_L1.xml" ) ;
