
# Data recording test
drg0 = trick.DRBinary("Ball")
for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
    for index in range(0,2) :
        var = "ball.output_" + param + "[" + str(index) + "]"
        drg0.add_variable(var)
#drg0.set_cycle(0.01)
drg0.freq = trick.DR_Always
trick_data_record.drd.add_group(drg0, trick.DR_No_Buffer)

trick_message.mpublisher.sim_name = "ball_sim"

read = 0.0
trick.add_read(read , """trick.checkpoint("chkpnt_pre_init")""")

read = 300.0
trick.add_read(read , """trick.checkpoint("chkpnt_300.0")""")

trick.exec_set_terminate_time(300.0)

