
exec(open("Modified_data/data_record.dr").read())

trick.var_allow_all_connections()

trick.sim_services.exec_set_terminate_time(300.0)
