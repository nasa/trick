
# new setIntegrator function to set the Integrator.
dyn_integloop.getIntegrator(trick.Runge_Kutta_4, 4)
dyn1_integloop.getIntegrator(trick.Runge_Kutta_4, 4)

#  Setting first_step_deriv and last_step_deriv is now a 
#  part of IntegLoopScheduler instead of the Integrator
#dyn_integloop.integ_sched.set_last_step_deriv(True)
#dyn1_integloop.integ_sched.set_first_step_deriv(False)

# Moving a sim_object to another IntegLoop
trick.add_read(3, """
dyn_integloop.integ_sched.add_sim_object(dyn1)
""")

trick.echo_jobs_on()
trick.stop(5)
