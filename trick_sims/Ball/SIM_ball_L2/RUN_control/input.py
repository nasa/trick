
exec(open("Modified_data/data_record.dr").read())

trick.var_allow_connections()
trick.var_resolve_hostname()

trick.sim_services.exec_set_terminate_time(300.0)
