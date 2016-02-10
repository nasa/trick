
# This one of several ways to include files from the input file
# execfile("Modified_data/auto_test.dr")
# execfile("Modified_data/data_record.dr")

ball.state.input.print_off = 1

# Set the freeze frame rate
# trick.exec_set_freeze_frame(0.10)

# Configure integration
ball.trick_technique = trick.User_Defined
ball.er7_technique = trick.Integration.RK2Midpoint
ball.use_trick_integ = False
end_time = 600000.0 / 2

# Set the stop time 
trick.exec_set_terminate_time(end_time)
