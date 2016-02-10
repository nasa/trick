
# $Id: input.py 1973 2011-10-11 15:11:16Z lin $

# This one of several ways to include files from the input file
execfile("Modified_data/auto_test.dr")
execfile("Modified_data/data_record.dr")

# Set the freeze frame rate
trick.exec_set_freeze_frame(0.10)

read = 55.0 
trick.add_read(read, "trick.ball_print(ball.state)") 

# Configure integration
ball.trick_technique = trick.Runge_Kutta_Fehlberg_78
ball.er7_technique = trick.Integration.Unspecified
ball.use_trick_integ = True

# Set the stop time 
trick.exec_set_terminate_time(300.0)

