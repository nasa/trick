import trick

from trick.unit_test import *

def main():

    trick.var_server_set_port(40000)
    trick.var_ascii()
    trick.real_time_enable()
    trick.exec_set_software_frame(0.01)

    trick.exec_set_terminate_time(1000.0)
    trick.var_server_set_source_address('localhost')

    varServerPort = trick.var_server_get_port()
    test_output = ( os.getenv("TRICK_HOME") + "/trick_test/SIM_test_varserv.xml" ) 
    command = 'os.system("./models/test_client/test_client_err1 ' + str(varServerPort) + ' --gtest_output=xml:' + test_output + ' &")'

    # Start the test client after everything has been initialized (hopefully)
    trick.add_read(1.0, command)

if __name__ == "__main__":
	main()

