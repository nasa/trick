#include <string.h>
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"

int main( int narg, char** args) {
    TCDevice connection;

    double x, y ;

    memset(&connection, '\0', sizeof(TCDevice)) ;

    connection.port = 9000;
    connection.hostname = (char*) malloc(16);

    strcpy(connection.hostname, "localhost");

    tc_connect(&connection);

    x = 123.45;
    y = 67.89 ;

    tc_write(&connection, (char*) &x, sizeof(double));
    tc_write(&connection, (char*) &y, sizeof(double));

    tc_disconnect(&connection) ;

    return 0;

}
