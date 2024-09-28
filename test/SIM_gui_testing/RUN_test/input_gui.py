exec(open("./Modified_Data/realtime.py").read())

# Open the SimControlPanel
simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

