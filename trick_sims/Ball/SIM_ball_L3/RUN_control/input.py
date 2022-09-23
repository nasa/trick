
exec(open("Modified_data/data_record.dr").read())


ball2.state.input.mass = trick.sim_services.attach_units("kg" , 10.0)
ball2.state.input.speed = trick.sim_services.attach_units("m/s" , 3.5)
ball2.state.input.elevation = trick.sim_services.attach_units("d" , 30.0) ;
ball2.state.input.position[0] = trick.sim_services.attach_units("m" , [5.0 , 5.0])

trick.sim_services.exec_set_terminate_time(300.0)
