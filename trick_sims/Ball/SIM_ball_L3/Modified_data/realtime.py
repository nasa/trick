
real_time.rt_sync.enable()
trick_sys.sched.set_software_frame(0.1)

trick_sys.sched.set_enable_freeze(True)
trick_sys.sched.set_freeze_command(True)

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)
