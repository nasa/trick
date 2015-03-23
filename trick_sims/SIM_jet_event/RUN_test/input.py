
#execfile("Modified_data/realtime.py")
execfile("Modified_data/cannon_aero.dr")

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

my_event = trick.new_event("my_event")
my_event.set_cycle(0.01) 
my_event.condition(0,"""dyn.baseball.pos[0] <= 9.0 """)
my_event.action(0,"""
dyn.baseball.jet_on = 1 
""")

trick.add_event(my_event)
my_event.activate()

trick.exec_set_terminate_time(1.0)
