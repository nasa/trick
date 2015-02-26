
# $Id: input.py 757 2010-06-22 18:40:57Z wwoodard $

execfile("Modified_data/cannon.dr")
dyn_integloop.getIntegrator(trick.Euler_Cromer, 4)
dyn_integloop.integ_sched.set_verbosity( 1)
trick.exec_set_terminate_time(5.2)
