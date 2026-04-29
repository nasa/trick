exec(open("Modified_data/Rocket.dr").read())
#exec(open("Modified_data/realtime.py").read())

trick.var_allow_connections()
trick.var_resolve_hostname()

trick.exec_set_terminate_time(20.0)
