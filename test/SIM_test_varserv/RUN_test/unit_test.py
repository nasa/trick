import trick

from trick.unit_test import *

def main():

	trick.var_server_set_port(40000)
	trick.var_ascii()
	trick.real_time_enable()
	trick.exec_set_software_frame(0.01)
	# trick.set_var_server_info_msg_on()
	trick.set_var_server_log_on()

	hostname = trick.var_server_get_hostname()

	trick.var_server_create_tcp_socket(hostname, 49000)
	trick.var_server_create_udp_socket(hostname, 48000)
	trick.var_server_create_multicast_socket('224.10.10.10',hostname, 47000)

	trick.exec_set_terminate_time(100.0)

	varServerPort = trick.var_server_get_port()
	test_output = ( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_varserv.xml" ) 
	command = 'os.system("./models/test_client/test_client ' + str(varServerPort) + ' --gtest_output=xml:' + test_output + ' &")'

	# Start the test client after everything has been initialized (hopefully)
	trick.add_read(1.0, command)

if __name__ == "__main__":
	main()

