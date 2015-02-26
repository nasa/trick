
# $Id: realtime.py 1082 2010-09-20 14:32:56Z wwoodard $

trick.frame_log_on()
trick.real_time_enable()
trick.exec_set_software_frame(0.1)
trick.itimer_enable()

trick.exec_set_enable_freeze(True)
trick.exec_set_freeze_command(True)

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)
