
exec(open("RUN_test/overrun.py").read())

# Disable real-time (if applicable)
trick.real_time_disable()
# Disable all GUIs
trick_sys.sched.set_freeze_command(False)
trick.remove_all_external_applications()

trick_utest.unit_tests.enable() ;
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_threads.xml" ) ;
