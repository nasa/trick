# Data recording
drg0 = trick.DRBinary("spring")
drg0.set_freq(trick.DR_Always)
drg0.enable()
drg0.set_cycle(0.01)
drg0.add_variable("spring.pos", "spring.pos")
drg0.add_variable("spring.vel", "spring.vel")
drg0.add_variable("spring.acc", "spring.acc")
trick.add_data_record_group( drg0 )

# Realtime
trick.frame_log_on()
trick.real_time_enable()
trick_sys.sched.set_software_frame(0.01)
trick.itimer_disable()
trick_sys.sched.set_enable_freeze(True)
trick_sys.sched.set_freeze_command(True)
simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)
trick.var_server_set_port(4545)

# Stop time
trick.exec_set_terminate_time(10.0)
