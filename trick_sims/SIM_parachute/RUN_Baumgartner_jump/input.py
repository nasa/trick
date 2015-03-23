#execfile("Modified_data/realtime.py")
execfile("Modified_data/parachutist.dr")

trick.TMM_reduced_checkpoint(0)
dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 4)

trick.stop(800)
