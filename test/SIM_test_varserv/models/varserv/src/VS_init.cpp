/******************************TRICK HEADER*************************************
PURPOSE:                     ( Test variable server )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( VS.o )
PROGRAMMERS:                 ( (Lindsay Landry) (L3) (9-12-2013) )
*******************************************************************************/
#include <iostream>
#include "../include/VS.hh"
#include "sim_services/VariableServer/include/variable_server_proto.h"
#include "sim_services/VariableServer/include/VariableServer.hh"

int VSTest::init() {
	char msg[256];

	port_num = var_server_get_port();
	hostest = "localhost";

	memset(&comm_device, '\0', sizeof(TCDevice));

	comm_device.hostname = const_cast<char*>(hostest);
	comm_device.port = port_num;

	//std::cout << bob.hostname << bob.port << std::endl;

	comm_device.disable_handshaking = TC_COMM_TRUE;
	comm_device.disabled = TC_COMM_FALSE;

	tc_connect(&comm_device);

	if ( tc_isValid (&comm_device) ) {
		printf ("connection is valid\n");
	} else {
		printf ("connection is NOT valid\n");
	}

	// default cycle rate
	cycle_rate = 0.01;
	snprintf(msg, sizeof(msg), "trick.var_cycle(%.2f)\n", cycle_rate);
	vs_write(msg);

	return (0);
}
