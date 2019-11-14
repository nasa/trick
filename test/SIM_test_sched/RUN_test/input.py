
#exec(open("Modified_data/data_record.dr").read())
#exec(open("Modified_data/realtime.py").read())

my_integ_loop.getIntegrator(trick.Runge_Kutta_4, 2)
trick.sim_services.exec_set_terminate_time(300.0)
#trick.echo_jobs_on();
