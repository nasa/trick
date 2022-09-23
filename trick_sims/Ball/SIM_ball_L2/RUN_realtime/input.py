
exec(open("Modified_data/data_record.dr").read())
exec(open("Modified_data/realtime.py").read())
exec(open("Modified_data/graphics.py").read())

trick.sim_services.exec_set_terminate_time(300.0)

