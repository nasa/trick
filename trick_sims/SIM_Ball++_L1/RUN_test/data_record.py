
trick_message.mpublisher.sim_name = "ball_sim"

#trick_sys.cmd_args.output_dir_timestamped_on() ;

# Data recording Ascii test
drg0 = trick.DRAscii("Ball")
for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
    for index in range(0,2) : 
        var = "ball.obj.state.output." + param + "[" + str(index) + "]" 
        drg0.add_variable(var)
drg0.set_cycle(1.00)
drg0.freq = trick.DR_Always
trick.add_data_record_group(drg0, trick.DR_No_Buffer)

# Data recording HDF5 test
# Using this flag as HDF5 might not be enabled
is_drg1_available = 0
try:
    drg1 = trick.DRHDF5("Ball2")
    is_drg1_available = 1
except:
    print "HDF5 is not enabled!"

if is_drg1_available == 1:
    for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
        for index in range(0,2) : 
            var = "ball.obj.state.output." + param + "[" + str(index) + "]" 
            drg1.add_variable(var)
    drg1.set_cycle(0.01)
    drg1.freq = trick.DR_Always
    trick.add_data_record_group(drg1, trick.DR_No_Buffer)

# Data recording binary test
drg3 = trick.DRBinary("Ball3")
for param in [ 'position' , 'velocity' , 'acceleration' , 'external_force' ] :
    for index in range(0,2) : 
        var = "ball.obj.state.output." + param + "[" + str(index) + "]" 
        drg3.add_variable(var)
drg3.set_cycle(0.01)
drg3.freq = trick.DR_Always
trick.add_data_record_group(drg3, trick.DR_No_Buffer)

read = 5.0
trick.add_read(read, "drg0.disable()")
if is_drg1_available == 1:
    trick.add_read(read, "drg1.disable()")
trick.add_read(read, "drg3.disable()")

read = 15.0
trick.add_read(read, "drg0.enable()")
if is_drg1_available == 1:
    trick.add_read(read, "drg1.enable()")
trick.add_read(read, "drg3.enable()")

trick.exec_set_terminate_time(300.0)

