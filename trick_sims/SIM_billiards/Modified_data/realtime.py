
trick.real_time_enable()
trick.exec_set_software_frame(0.01)
trick.itimer_enable()

trick.exec_set_enable_freeze(True)
# trick.exec_set_freeze_command(True)

# Print stack trace if signal is caught. default True
trick.exec_set_stack_trace(False)
# Attach a debugger to the process if a signal is caught. default False
trick.exec_set_attach_debugger(False)

simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)
