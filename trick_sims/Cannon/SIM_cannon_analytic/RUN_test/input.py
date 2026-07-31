exec(open("Modified_data/cannon.dr").read())
exec(open("Modified_data/realtime.py").read())

trick.var_allow_connections()
trick.var_resolve_hostname()

trick.exec_set_terminate_time(5.2)

