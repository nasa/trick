
execfile("Modified_data/cannon.dr")

trick.exec_set_sim_object_onoff("server", False) ;

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 4)
trick.exec_set_terminate_time(5.2)
