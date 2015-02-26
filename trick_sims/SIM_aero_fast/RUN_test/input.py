
# $Id: input.py 1051 2010-09-07 21:14:56Z wwoodard $

execfile("Modified_data/realtime.py")
execfile("Modified_data/cannon_aero.dr")

dyn.baseball.pos[0] = 16.0
dyn.baseball.pos[1] = 0.1
dyn.baseball.pos[2] = 2.0

dyn.baseball.vel[0] = -30.0
dyn.baseball.vel[1] = -0.1
dyn.baseball.vel[2] = 1.0

dyn.baseball.theta = trick.attach_units("d",-90.0)
dyn.baseball.phi = trick.attach_units("d",1.0)
dyn.baseball.omega0 = trick.attach_units("rev/s",-30.0)

trickView = trick.TrickView()
trickView.set_auto_open_file("cannon.tv")
trick.add_external_application(trickView)

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 6)
trick.exec_set_terminate_time(5.2)
