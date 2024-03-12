exec(open("./Modified_Data/realtime.py").read())

# Open the SimControlPanel
simControlPanel = trick.SimControlPanel()
trick.add_external_application(simControlPanel)

# Save the Connection Info <host,port> to a file
host = trick.var_server_get_hostname()
port = trick.var_server_get_port()