
# $Id: unit_test.py 1003 2010-08-25 21:18:24Z dpanter $

dyn.baseball.pos[0] = 16.0
dyn.baseball.pos[1] = 0.1 
dyn.baseball.pos[2] = 2.0 

dyn.baseball.vel[0] = -30.0
dyn.baseball.vel[1] = -0.1
dyn.baseball.vel[2] = 1.0 

dyn.baseball.theta = trick.attach_units("d",-90.0)
dyn.baseball.phi = trick.attach_units("d",1.0) 
dyn.baseball.omega0 = trick.attach_units("rev/s",30.0)

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)

trick.add_read(0.4 , """
dyn.baseball.jet_on = 1 """)

trick.exec_set_terminate_time(1.0)
