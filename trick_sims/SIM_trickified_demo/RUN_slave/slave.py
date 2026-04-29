
trick_message.mpublisher.sim_name = "slave"

#this will be turned off if the sim is a slave
trick.real_time_enable()

trick.exec_set_software_frame(0.1)
trick.exec_set_freeze_frame(2.00)
#trick.exec_set_freeze_command(True)

new_connection = trick.MSSocket()
trick_master_slave.slave.set_connection_type(new_connection)
trick_master_slave.slave.sync_error_terminate = 1

#trick.message_file_set_enabled(False)

