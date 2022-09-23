
# $Id: input.py 1973 2011-10-11 15:11:16Z lin $

# This one of several ways to include files from the input file
exec(open("Modified_data/auto_test.dr").read())
exec(open("Modified_data/data_record.dr").read())

# Set the freeze frame rate
trick.exec_set_freeze_frame(0.10)

read = 55.0 
trick.add_read(read, "trick.ball_print(ball.state)") 

# Configure integration
ball.trick_technique = trick.Modified_Midpoint_4
ball.er7_technique = trick.Integration.Unspecified
ball.use_trick_integ = False

# Set the stop time 
trick.exec_set_terminate_time(300.0)

