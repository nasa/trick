execfile("RUN_test/input.py")

# Disable real-time (if applicable)
trick.real_time_disable()
# Disable all GUIs
trick_sys.sched.set_freeze_command(False)
trick.remove_all_external_applications()

trick_utest.unit_tests.enable() ;
trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_sun.xml" ) ;

trick.exec_set_job_onoff("sun_predictor.sun_pred_fast_display", 1, 0)
trick.exec_set_job_cycle("sun_predictor.sun_pred_slow_display", 1, 1000.0)

trick.stop(86400.0)

