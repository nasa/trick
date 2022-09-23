
exec(open("Modified_data/cannon.dr").read())
dyn_integloop.getIntegrator(trick.Euler_Cromer, 4)
dyn_integloop.integ_sched.set_verbosity( 1)
trick.exec_set_terminate_time(5.2)
