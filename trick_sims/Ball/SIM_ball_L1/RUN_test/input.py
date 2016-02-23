
# This one of several ways to include files from the input file
execfile("Modified_data/auto_test.dr")
execfile("Modified_data/data_record.dr")

#my_integ_loop.integ_sched.integ_ptr = trick.getIntegrator( trick.Runge_Kutta_2, 4, 1.0);
my_integ_loop.getIntegrator(trick.Runge_Kutta_2, 4)

# Set the freeze frame rate
trick.exec_set_freeze_frame(0.10)

read = 55.0
trick.add_read(read, "trick.ball_print(ball.state)")

# Set the stop time
trick.exec_set_terminate_time(300.0)

