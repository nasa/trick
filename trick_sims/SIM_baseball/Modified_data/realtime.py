
trick.frame_log_on()
real_time.rt_sync.enable()
trick_sys.sched.set_software_frame(0.01)
real_time.itimer.enable()

trick_sys.sched.enable_freeze = True
trick_sys.sched.freeze_command = True

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)
