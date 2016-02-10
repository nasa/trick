
# This one of several ways to include files from the input file
# execfile("Modified_data/auto_test.dr")
# execfile("Modified_data/data_record.dr")

ball.state.input.print_off = 1

# Set the freeze frame rate
# trick.exec_set_freeze_frame(0.10)

# Configure integration
ball.trick_technique = trick.TRICK_TECHNIQUE
ball.er7_technique = trick.ER7_TECHNIQUE
ball.use_trick_integ = USE_TRICK_INTEG
end_time = 600000.0 / DERIVS_PER_STEP

# Set the stop time 
trick.exec_set_terminate_time(end_time)
