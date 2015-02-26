
# set the sim to run realtime
trick.real_time_enable()
trick.itimer_enable()

# set the software frame and freeze_frame
trick.exec_set_software_frame(0.1)
trick.exec_set_freeze_frame(2.00)

trick_message.mpublisher.sim_name = "master"

new_connection = trick.MSSocket()
new_slave = trick.SlaveInfo()
new_slave.set_connection_type(new_connection)
new_slave.sim_path = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1"
#if trick.dmtcp_is_enabled() == 1:
#    new_slave.S_main_name = "dmtcp_checkpoint --checkpoint-open-files  ./S_main_${TRICK_HOST_CPU}.exe"
#print "hey " , trick.dmtcp_is_enabled()
new_slave.run_input_file = "RUN_slave/slave.py"
new_slave.sync_error_terminate = 1
trick_master_slave.master.add_slave(new_slave)
trick_master_slave.master.enable()

trick.sim_control_panel_set_enabled(True)

trick.exec_set_freeze_command(True)
# test freezing on frame boundaries
#trick.freeze(5.999999)
#trick.add_read(4.9999, "trick.freeze()")

print "Number of slaves...", trick_master_slave.master.num_slaves
print "The 1st slave sim path...", trick_master_slave.master.slaves[0].sim_path

trick.exec_set_terminate_time(300.0)

