
# $Id: input.py 757 2010-06-22 18:40:57Z wwoodard $

#execfile("Modified_data/realtime.py")
execfile("Modified_data/cannon.dr")

dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 4)
trick.exec_set_terminate_time(5.2)
