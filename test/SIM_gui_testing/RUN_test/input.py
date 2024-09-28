exec(open("./Modified_Data/realtime.py").read())

# Save the Connection Info <host,port> to a file
host = trick.var_server_get_hostname()
port = trick.var_server_get_port()

f = open("socket_info", "w")
f.write(str(host) + ":" + str(port))
f.close()

