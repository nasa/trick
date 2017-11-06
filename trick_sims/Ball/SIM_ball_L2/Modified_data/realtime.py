
trick.frame_log_off()
trick.real_time_enable()
trick_sys.sched.set_software_frame(0.01)
trick.itimer_disable()

trick_sys.sched.set_enable_freeze(True)
trick_sys.sched.set_freeze_command(True)

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

#trick_sys.sched.threads[1].rt_cpu_number   = 1
#trick_sys.sched.threads[1].rt_lock_memory  = Yes
#trick_sys.sched.threads[1].rt_nond_pri     = Yes
#trick_sys.sched.threads[1].rt_priority     = 1
#trick_sys.sched.threads[1].rt_lock_to_cpu  = Yes

