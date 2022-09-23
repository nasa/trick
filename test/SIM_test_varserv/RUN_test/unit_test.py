import trick
import socket

from trick.unit_test import *

def main():

	trick.var_server_set_port(40000)
	trick.var_ascii()

	#trick.set_var_server_info_msg_on()

	trick_utest.unit_tests.enable() ;
	trick_utest.unit_tests.set_file_name( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_varserv.xml" ) 
	trick_utest.unit_tests.set_test_name( "VariableServerTest" )

	TRICK_EXPECT_EQ(trick.var_server_get_port(), 40000, "VariableServerTest", "SetPortNumber")

	trick.exec_set_terminate_time(3000.0)

if __name__ == "__main__":
	main()

