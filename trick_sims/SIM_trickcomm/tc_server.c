#define send_hs fprintf

#include <stdio.h>
#include <string.h>
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"

int main (int narg, char** args)
{
    double x, y;
    int nbytes ;


    TCDevice listen_device ;
    TCDevice connection ;

    memset(&listen_device, '\0', sizeof(TCDevice)) ;
    memset(&connection,    '\0', sizeof(TCDevice)) ;

    listen_device.port = 9000;
    tc_init(&listen_device) ;
    tc_accept(&listen_device, &connection) ;

    while(tc_isValid(&connection)) {
        nbytes = tc_read(&connection, (char*) &x, sizeof(double));
        nbytes = tc_read(&connection, (char*) &y, sizeof(double));
        fprintf(stderr, "x=%lf y=%lf \n", x, y) ;
    }

    return 0 ;
}
    
