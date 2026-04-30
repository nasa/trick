
# set the sim to run realtime
trick.real_time_enable()
trick.itimer_enable()

# set the software frame and freeze_frame
trick.exec_set_software_frame(0.1)
trick.exec_set_freeze_frame(2.00)

trick_message.mpublisher.sim_name = "master"

new_connection1 = trick.MSSharedMem()
new_connection1.tsm_dev.key_file = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1/RUN_shm_slaves/slave1_shm1.py"
new_slave1 = trick.SlaveInfo()
new_slave1.set_connection_type(new_connection1)
new_slave1.sim_path = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1"
new_slave1.run_input_file = "RUN_shm_slaves/slave1_shm1.py"
new_slave1.remote_shell_config_file = "local_trick.sh"
new_slave1.sync_error_terminate = 1
trick_master_slave.master.add_slave(new_slave1)

new_connection2 = trick.MSSharedMem()
new_connection2.tsm_dev.key_file = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1/RUN_shm_slaves/slave2_shm2.py"
new_connection2.tsm_dev.proj_id = 5
new_slave2 = trick.SlaveInfo()
new_slave2.set_connection_type(new_connection2)
new_slave2.sim_path = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1"
new_slave2.run_input_file = "RUN_shm_slaves/slave2_shm2.py"
new_slave2.remote_shell_config_file = "local_trick.sh"
new_slave2.sync_error_terminate = 1
trick_master_slave.master.add_slave(new_slave2)

new_connection3 = trick.MSSocket()
new_slave3 = trick.SlaveInfo()
new_slave3.set_connection_type(new_connection3)
new_slave3.sim_path = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1"
new_slave3.run_input_file = "RUN_socket_slaves/slave3_sock1.py"
new_slave3.remote_shell_config_file = "local_trick.sh"
new_slave3.sync_error_terminate = 1
trick_master_slave.master.add_slave(new_slave3)

new_connection4 = trick.MSSocket()
new_slave4 = trick.SlaveInfo()
new_slave4.set_connection_type(new_connection4)
new_slave4.sim_path = trick.get_trick_env("TRICK_HOME") + "/trick_sims/SIM_Ball++_L1"
new_slave4.run_input_file = "RUN_socket_slaves/slave4_sock2.py"
new_slave4.remote_shell_config_file = "local_trick.sh"
new_slave4.sync_error_terminate = 1
trick_master_slave.master.add_slave(new_slave4)

trick_master_slave.master.enable()

trick.sim_control_panel_set_enabled(True)

trick.exec_set_freeze_command(True)
# test freezing on frame boundaries
#trick.freeze(5.999999)
#trick.add_read(4.9999, "trick.freeze()")

print(f"Number of slaves...{trick_master_slave.master.num_slaves}")
for ii in range(trick_master_slave.master.num_slaves):
   print(f"Slave {ii} sim path...{trick_master_slave.master.slaves[ii].sim_path}/{trick_master_slave.master.slaves[ii].run_input_file}")

trick.exec_set_terminate_time(300.0)

