
# $Id: realtime_jet.py 1012 2010-08-26 21:00:09Z dpanter $

trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.01)
trick.itimer_enable()

trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)
