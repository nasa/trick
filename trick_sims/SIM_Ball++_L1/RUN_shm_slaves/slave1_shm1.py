
trick_message.mpublisher.sim_name = "slave1_shm1"

#this will be turned off if the sim is a slave
trick.real_time_enable()

trick.exec_set_software_frame(0.1)
trick.exec_set_freeze_frame(2.00)
#trick.exec_set_freeze_command(True)

new_connection = trick.MSSharedMem()
new_connection.tsm_dev.key_file = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1/RUN_shm_slaves/slave1_shm1.py"
trick_master_slave.slave.set_connection_type(new_connection)
trick_master_slave.slave.sync_error_terminate = 1

#trick.message_file_set_enabled(False)

