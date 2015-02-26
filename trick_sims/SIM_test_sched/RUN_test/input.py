
# $Id: input.py 2553 2012-08-30 22:00:40Z alin $

#execfile("Modified_data/data_record.dr")
#execfile("Modified_data/realtime.py")

my_integ_loop.getIntegrator(trick.Runge_Kutta_4, 2)
trick.sim_services.exec_set_terminate_time(300.0)
#trick.echo_jobs_on();
