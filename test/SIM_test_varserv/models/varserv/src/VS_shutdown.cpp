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
#include "trick/exec_proto.h"

int VSTest::shutdown() {
	std::cout << "Shutting down with status: " << status << " Message: " << status_messages[status] << std::endl;
    exec_terminate_with_return( status , __FILE__ , __LINE__ , status_messages[status] ) ;
	
	return(0);
}
