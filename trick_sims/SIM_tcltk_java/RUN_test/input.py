
execfile("Modified_data/cannon_aero.dr")
execfile("Modified_data/realtime_jet.py")

dyn.baseball.pos[0] = 0.0
dyn.baseball.pos[1] = 0.0
dyn.baseball.pos[2] = 0.0

dyn.baseball.vel[0] = 43.30
dyn.baseball.vel[1] = 0.0
dyn.baseball.vel[2] = 25.0

dyn.baseball.theta  = trick.sim_services.attach_units("d" , -90.0)
dyn.baseball.phi    = trick.sim_services.attach_units("d" , 1.0)
dyn.baseball.omega0 = trick.sim_services.attach_units("rev/s" , 30.0)

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)

trick.sim_services.exec_set_terminate_time(10.0)
